# supla-mqtt-client

This project combines SUPLA with MQTT. Its basic functionality is publishing the status of SUPLA channels to the MQTT server and responding to MQTT server messages to control SUPLA channels. The output and input message patterns are configurable in the state.yaml and command.yaml files

# Building
To build the project You need SSL library first. It can be installed by running command:
```
sudo apt-get install libssl-dev
```


```
git clone https://github.com/SUPLA/supla-core.git
cd supla-core/supla-mqtt-client/Release
sudo make clean
sudo make all
```

If above command returns with directory supla-mqtt-client not found erro you must change branch to mqtt-experimental

```
git checkout -b mqtt-experimental remotes/origin/mqtt-experimental
```

# Configuration

The project contains three configuration files. They are located in the config directory.
The most important is the config_template.yaml file

IMPORTANT!!! test.mosquitto.org is available to everyone who connects to it. 
You should not use it with Your production supla instance!

```
sudo nano config_template.yaml 

mqtt:
  host: 'mqtthost' # MQTT server hostname
  port: 1883 # MQTT server port
  commands_file_path: '/home/pi/supla-mqtt-client/config/command.yaml' # absolute path to the command.yaml configuration file
  states_file_path: '/home/pi/supla-mqtt-client/config/state.yaml' # absolute path to the state.yaml configuration file
  username: '' # MQTT username if the server is secured in this way. if You don't use it delete this line
  password: '' # MQTT password if the server is secured in this way. uf You don't use it delete this line
supla:
  port: 2016 # SUPLA server port
  host: 'localhost' # SUPLA server hostname
  location: 2 # access id from SUPLA Cloud
  password: 'password' # password for the given access identifier
```


Then modify the state.yaml file if needed. The structure of this file is as follows:
```
 - channel_type: 40 # SUPLA channel type
    state_topic: 'supla/channels/status/thermometer/$id' #topic under which the channel status will be published
    payload_template: '{"caption": "$caption$", "temperature": $temperature$}' #MQTT message payload template
```

Depending on the type of channel, different macros are available that can be used in templates. The most important of them are:

```
$id$ - SUPLA id will be substituted for this macro
$caption$ - the SUPLA channel name will be substituted for this macro
$temperature$ - the temperature value will be substituted for this macro (for the channel in which the temperature occurs)
$humidity$ - the humidity value will be substituted for this macro (for the channel in which humidity occurs)
```
The rest of the macros are described in the state.yaml file

# Running
```
./supla-mqtt-client -config config_template.yaml
```
