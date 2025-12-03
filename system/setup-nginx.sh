#!/bin/bash
sudo systemctl stop nginx
sudo cp -arfv nginx/nginx.conf /etc/nginx/nginx.conf
sudo cp -arfv nginx/security.conf /etc/nginx/conf.d/security.conf
sudo rm -rf /etc/nginx/sites-available/*
sudo rm -rfv /var/www/html/*
sudo cp -arfv nginx/html/* /var/www/html/

sudo chown -R www-data:www-data /var/www/html/
sudo nginx -t
sudo systemctl start nginx