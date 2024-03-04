#!/bin/bash

echo "Enter new IP and old IP:"
read -p "Old IP: " old_ip
read -p "New IP: " new_ip
echo "Patching..."

# Use double quotes around variables and specify the file to edit
sed -i "s/$old_ip/$new_ip/g" static/beacon
sed -i "s/$old_ip/$new_ip/g" static/beacon.exe
echo "Finished!"
