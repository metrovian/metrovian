#!/bin/bash
sudo systemctl disable wpa_supplicant@wlan0
sudo systemctl stop wpa_supplicant@wlan0
sudo systemctl mask wpa_supplicant@wlan0

sudo systemctl disable wpa_supplicant
sudo systemctl stop wpa_supplicant
sudo systemctl mask wpa_supplicant

sudo systemctl stop dnsmasq
sudo systemctl stop dhcpcd
sudo systemctl stop hostapd
sudo systemctl unmask hostapd

sudo cp -arfv hostapd/dnsmasq.conf /etc/dnsmasq.conf
sudo cp -arfv hostapd/dhcpcd.conf /etc/dhcpcd.conf
sudo cp -arfv hostapd/hostapd.conf /etc/hostapd/hostapd.conf
sudo cp -arfv hostapd/hostapd /etc/default/hostapd

sudo systemctl start dnsmasq
sudo systemctl start dhcpcd
sudo systemctl start hostapd

sudo systemctl status dnsmasq --no-pager
sudo systemctl status dhcpcd --no-pager
sudo systemctl status hostapd --no-pager

sudo systemctl enable dnsmasq
sudo systemctl enable dhcpcd
sudo systemctl enable hostapd