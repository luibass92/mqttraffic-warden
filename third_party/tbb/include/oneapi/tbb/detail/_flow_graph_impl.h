/*
    Copyright (c) 2005-2021 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef __TBB_flow_graph_impl_H
#define __TBB_flow_graph_impl_H

// #include "../config.h"
#include "_task.h"
#include "../task_group.h"
#include "../task_arena.h"
#include "../flow_graph_abstractions.h"

#include "../concurrent_priority_queue.h"

#include <list>

namespace tbb {
namespace detail {

namespace d1 {

class graph_task;
static graph_task* const SUCCESSFULLY_ENQUEUED = (graph_task*)-1;
typedef unsigned int node_priority_t;
static const node_priority_t no_priority = node_priority_t(0);

class graph;
class graph_node;

template <typename GraphContainerType, typename GraphNodeType>
class graph_iterator {
    friend class graph;
    friend class graph_node;
public:
    typedef size_t size_type;
    typedef GraphNodeType value_type;
    typedef GraphNodeType* pointer;
    typedef GraphNodeType& reference;
    typedef const GraphNodeType& const_reference;
    typedef std::forward_iterator_tag iterator_category;

    //! Copy constructor
    graph_iterator(const graph_iterator& other) :
        my_graph(other.my_graph), current_node(other.current_node)
    {}

    //! Assignment
    graph_iterator& operator=(const graph_iterator& other) {
        if (this != &other) {
            my_graph = other.my_graph;
            current_node = other.current_node;
        }
        return *this;
    }

    //! Dereference
    reference operator*() const;

    //! Dereference
    pointer operator->() const;

    //! Equality
    bool operator==(const graph_iterator& other) const {
        return ((my_graph == other.my_graph) && (current_node == other.current_node));
    }

#if !__TBB_CPP20_COMPARISONS_PRESENT
    //! Inequality
    bool operator!=(const graph_iterator& other) const { return !(operator==(other)); }
#endif

    //! Pre-increment
    graph_iterator& operator++() {
        internal_forward();
        return *this;
    }

    //! Post-increment
    graph_iterator operator++(int) {
        graph_iterator result = *this;
        operator++();
        return result;
    }

private:
    // the graph over which we are iterating
    GraphContainerType *my_graph;
    // pointer into my_graph's my_nodes list
    pointer current_node;

    //! Private initializing constructor for begin() and end() iterators
    graph_iterator(GraphContainerType *g, bool begin);
    void internal_forward();
};  // class graph_iterator

// flags to modify the behavior of the graph reset().  Can be combined.
enum reset_flags {
    rf_reset_protocol = 0,
    rf_reset_bodies = 1 << 0,  // delete the current node body, reset to a copy of the initial node body.
    rf_clear_edges = 1 << 1   // delete edges
};

void activate_graph(graph& g);
void deactivate_graph(graph& g);
bool is_graph_active(graph& g);
graph_task* prioritize_task(graph& g, graph_task& arena_task);
void spawn_in_graph_arena(graph& g, graph_task& arena_task);
void enqueue_in_graph_arena(graph &g, graph_task& arena_task);

class graph;

//! Base class for tasks generated by graph nodes.
class graph_task : public task {
public:
    graph_task(graph& g, small_object_allocator& allocator
               , node_priority_t node_priority = no_priority
    )
        : my_graph(g)
        , priority(node_priority)
        , my_allocator(allocator)
    {}
    graph& my_graph; // graph instance the task belongs to
    // TODO revamp: rename to my_priority
    node_priority_t priority;
    void destruct_and_deallocate(const execution_data& ed);
    task* cancel(execution_data& ed) override;
protected:
    void finalize(const execution_data& ed);
private:
    // To organize task_list
    graph_task* my_next{ nullptr };
    small_object_allocator my_allocator;
    // TODO revamp: elaborate internal interfaces to avoid friends declarations
    friend class graph_task_list;
    friend graph_task* prioritize_task(graph& g, graph_task& gt);
};

struct graph_task_comparator {
    bool operator()(const graph_task* left, const graph_task* right) {
        return left->priority < right->priority;
    }
};

typedef tbb::concurrent_priority_queue<graph_task*, graph_task_comparator> graph_task_priority_queue_t;

class priority_task_selector : public task {
public:
    priority_task_selector(graph_task_priority_queue_t& priority_queue, small_object_allocator& allocator)
        : my_priority_queue(priority_queue), my_allocator(allocator), my_task() {}
    task* execute(execution_data& ed) override {
        next_task();
        __TBB_ASSERT(my_task, nullptr);
        task* t_next = my_task->execute(ed);
        my_allocator.delete_object(this, ed);
        return t_next;
    }
    task* cancel(execution_data& ed) override {
        if (!my_task) {
            next_task();
        }
        __TBB_ASSERT(my_task, nullptr);
        task* t_next = my_task->cancel(ed);
        my_allocator.delete_object(this, ed);
        return t_next;
    }
private:
    void next_task() {
        // TODO revamp: hold functors in priority queue instead of real tasks
        bool result = my_priority_queue.try_pop(my_task);
        __TBB_ASSERT_EX(result, "Number of critical tasks for scheduler and tasks"
            " in graph's priority queue mismatched");
        __TBB_ASSERT(my_task && my_task != SUCCESSFULLY_ENQUEUED,
            "Incorrect task submitted to graph priority queue");
        __TBB_ASSERT(my_task->priority != no_priority,
            "Tasks from graph's priority queue must have priority");
    }

    graph_task_priority_queue_t& my_priority_queue;
    small_object_allocator my_allocator;
    graph_task* my_task;
};

template <typename Receiver, typename Body> class run_and_put_task;
template <typename Body> class run_task;

//********************************************************************************
// graph tasks helpers
//********************************************************************************

//! The list of graph tasks
class graph_task_list : no_copy {
private:
    graph_task* my_first;
    graph_task** my_next_ptr;
public:
    //! Construct empty list
    graph_task_list() : my_first(nullptr), my_next_ptr(&my_first) {}

    //! True if list is empty; false otherwise.
    bool empty() const { return !my_first; }

    //! Push task onto back of list.
    void push_back(graph_task& task) {
        task.my_next = nullptr;
        *my_next_ptr = &task;
        my_next_ptr = &task.my_next;
    }

    //! Pop the front task from the list.
    graph_task& pop_front() {
        __TBB_ASSERT(!empty(), "attempt to pop item from empty task_list");
        graph_task* result = my_first;
        my_first = result->my_next;
        if (!my_first) {
            my_next_ptr = &my_first;
        }
        return *result;
    }
};

//! The graph class
/** This class serves as a handle to the graph */
class graph : no_copy, public graph_proxy {
    friend class graph_node;

    void prepare_task_arena(bool reinit = false) {
        if (reinit) {
            __TBB_ASSERT(my_task_arena, "task arena is NULL");
            my_task_arena->terminate();
            my_task_arena->initialize(task_arena::attach());
        }
        else {
            __TBB_ASSERT(my_task_arena == NULL, "task arena is not NULL");
            my_task_arena = new task_arena(task_arena::attach());
        }
        if (!my_task_arena->is_active()) // failed to attach
            my_task_arena->initialize(); // create a new, default-initialized arena
        __TBB_ASSERT(my_task_arena->is_active(), "task arena is not active");
    }

public:
    //! Constructs a graph with isolated task_group_context
    graph();

    //! Constructs a graph with use_this_context as context
    explicit graph(task_group_context& use_this_context);

    //! Destroys the graph.
    /** Calls wait_for_all, then destroys the root task and context. */
    ~graph();

    //! Used to register that an external entity may still interact with the graph.
    /** The graph will not return from wait_for_all until a matching number of release_wait calls is
    made. */
    void reserve_wait() override;

    //! Deregisters an external entity that may have interacted with the graph.
    /** The graph will not return from wait_for_all until all the number of reserve_wait calls
    matches the number of release_wait calls. */
    void release_wait() override;

    //! Wait until graph is idle and the number of release_wait calls equals to the number of
    //! reserve_wait calls.
    /** The waiting thread will go off and steal work while it is blocked in the wait_for_all. */
    void wait_for_all() {
        cancelled = false;
        caught_exception = false;
        try_call([this] {
            my_task_arena->execute([this] {
                wait(my_wait_context, *my_context);
            });
            cancelled = my_context->is_group_execution_cancelled();
        }).on_exception([this] {
            my_context->reset();
            caught_exception = true;
            cancelled = true;
        });
        // TODO: the "if" condition below is just a work-around to support the concurrent wait
        // mode. The cancellation and exception mechanisms are still broken in this mode.
        // Consider using task group not to re-implement the same functionality.
        if (!(my_context->traits() & task_group_context::concurrent_wait)) {
            my_context->reset();  // consistent with behavior in catch()
        }
    }

    // TODO revamp: consider adding getter for task_group_context.

    // ITERATORS
    template<typename C, typename N>
    friend class graph_iterator;

    // Graph iterator typedefs
    typedef graph_iterator<graph, graph_node> iterator;
    typedef graph_iterator<const graph, const graph_node> const_iterator;

    // Graph iterator constructors
    //! start iterator
    iterator begin();
    //! end iterator
    iterator end();
    //! start const iterator
    const_iterator begin() const;
    //! end const iterator
    const_iterator end() const;
    //! start const iterator
    const_iterator cbegin() const;
    //! end const iterator
    const_iterator cend() const;

    // thread-unsafe state reset.
    void reset(reset_flags f = rf_reset_protocol);

    //! cancels execution of the associated task_group_context
    void cancel();

    //! return status of graph execution
    bool is_cancelled() { return cancelled; }
    bool exception_thrown() { return caught_exception; }

private:
    wait_context my_wait_context;
    task_group_context *my_context;
    bool own_context;
    bool cancelled;
    bool caught_exception;
    bool my_is_active;

    graph_node *my_nodes, *my_nodes_last;

    tbb::spin_mutex nodelist_mutex;
    void register_node(graph_node *n);
    void remove_node(graph_node *n);

    task_arena* my_task_arena;

    graph_task_priority_queue_t my_priority_queue;

    friend void activate_graph(graph& g);
    friend void deactivate_graph(graph& g);
    friend bool is_graph_active(graph& g);
    friend graph_task* prioritize_task(graph& g, graph_task& arena_task);
    friend void spawn_in_graph_arena(graph& g, graph_task& arena_task);
    friend void enqueue_in_graph_arena(graph &g, graph_task& arena_task);

    friend class task_arena_base;

};  // class graph

inline void graph_task::destruct_and_deallocate(const execution_data& ed) {
    auto allocator = my_allocator;
    // TODO: investigate if direct call of derived destructor gives any benefits.
    this->~graph_task();
    allocator.deallocate(this, ed);
}

inline void graph_task::finalize(const execution_data& ed) {
    graph& g = my_graph;
    destruct_and_deallocate(ed);
    g.release_wait();
}

inline task* graph_task::cancel(execution_data& ed) {
    finalize(ed);
    return nullptr;
}

//********************************************************************************
// end of graph tasks helpers
//********************************************************************************


#if __TBB_PREVIEW_FLOW_GRAPH_NODE_SET
class get_graph_helper;
#endif

//! The base of all graph nodes.
class graph_node : no_copy {
    friend class graph;
    template<typename C, typename N>
    friend class graph_iterator;

#if __TBB_PREVIEW_FLOW_GRAPH_NODE_SET
    friend class get_graph_helper;
#endif

protected:
    graph& my_graph;
    graph& graph_reference() const {
        // TODO revamp: propagate graph_reference() method to all the reference places.
        return my_graph;
    }
    graph_node* next = nullptr;
    graph_node* prev = nullptr;
public:
    explicit graph_node(graph& g);

    virtual ~graph_node();

protected:
    // performs the reset on an individual node.
    virtual void reset_node(reset_flags f = rf_reset_protocol) = 0;
};  // class graph_node

inline void activate_graph(graph& g) {
    g.my_is_active = true;
}

inline void deactivate_graph(graph& g) {
    g.my_is_active = false;
}

inline bool is_graph_active(graph& g) {
    return g.my_is_active;
}

inline graph_task* prioritize_task(graph& g, graph_task& gt) {
    if( no_priority == gt.priority )
        return &gt;

    //! Non-preemptive priority pattern. The original task is submitted as a work item to the
    //! priority queue, and a new critical task is created to take and execute a work item with
    //! the highest known priority. The reference counting responsibility is transferred (via
    //! allocate_continuation) to the new task.
    task* critical_task = gt.my_allocator.new_object<priority_task_selector>(g.my_priority_queue, gt.my_allocator);
    __TBB_ASSERT( critical_task, "bad_alloc?" );
    g.my_priority_queue.push(&gt);
    using tbb::detail::d1::submit;
    submit( *critical_task, *g.my_task_arena, *g.my_context, /*as_critical=*/true );
    return nullptr;
}

//! Spawns a task inside graph arena
inline void spawn_in_graph_arena(graph& g, graph_task& arena_task) {
    if (is_graph_active(g)) {
        task* gt = prioritize_task(g, arena_task);
        if( !gt )
            return;

        __TBB_ASSERT(g.my_task_arena && g.my_task_arena->is_active(), NULL);
        submit( *gt, *g.my_task_arena, *g.my_context
#if __TBB_PREVIEW_CRITICAL_TASKS
                , /*as_critical=*/false
#endif
        );
    }
}

// TODO revamp: unify *_in_graph_arena functions

//! Enqueues a task inside graph arena
inline void enqueue_in_graph_arena(graph &g, graph_task& arena_task) {
    if (is_graph_active(g)) {
        __TBB_ASSERT( g.my_task_arena && g.my_task_arena->is_active(), "Is graph's arena initialized and active?" );

        // TODO revamp: decide on the approach that does not postpone critical task
        if( task* gt = prioritize_task(g, arena_task) )
            submit( *gt, *g.my_task_arena, *g.my_context, /*as_critical=*/false);
    }
}

} // namespace d1
} // namespace detail
} // namespace tbb

#endif // __TBB_flow_graph_impl_H
