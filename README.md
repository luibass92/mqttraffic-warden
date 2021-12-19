# MQTTrafficWarden (MQTTW)
[![Commitizen friendly](https://img.shields.io/badge/commitizen-friendly-brightgreen.svg)](http://commitizen.github.io/cz-cli/)

## Overview
This repo contains the source code of MQTTrafficWarden (MQTTW from now on).<br />

In the world of IoT one of the most used communication protocol is [MQTT](https://mqtt.org/): it's a very lightweight protocol with high throughput that allow us to interconnect different devices through a central message broker which routes the messages incoming from publishers to the right topics on which subscribers are listening.<br />

A common problem though, is that each device has its own message format (comonly a JSON message), and this makes the development of central brain systems that have to process all of these messages a really tough work. Often the solution is to create "adaptors" that transform each incoming message in a standardized message format before the central brain can process it.<br />

It is obvious to say that to create these "adaptors" takes a lot of time and effort and we have to create a new adaptor for each device we want to integrate to our system.<br />
The purpose of MQTTW is to do this work for us; all we need to do is to configure the software through a configuration JSON file and it will take the incoming messages from an arbitrary number of MQTT topics, eventually transform them through an arbitrary number of stream transformers, and re-publish them in output on an arbitrary number of MQTT topics.</br >
This will strongly simplify our job potentially making any message compatible with an arbitrary standard form that has been previously defined.

## This repo is [commitizen](https://github.com/commitizen) friendly
If you want to contribute using commitizen guidelines, you can just commit using the `npm run commit` command.<br />
It is also possible to setup a traditional git hook or a Husky configuration as explained on [commitizen official page](https://github.com/commitizen/cz-cli#optional-running-commitizen-on-git-commit).
