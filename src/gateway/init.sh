#!/bin/sh 

# 当转移到一个新的网关上时，执行此初使化

echo "{\"updated\":\"0\", \"mwid\":\"\", \"hwid\":\"\"}" >cfg/gw_json.cfg 
rm -f cfg/*map.cfg
