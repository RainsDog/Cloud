#!/bin/sh

dev_addr=0

while [ 1 ]
do
	let dev_addr+=1 
	i2c_read 0x1 $dev_addr 0 0 1 1 1
done