#!/usr/bin/env python

from requests import get, post

url = "http://127.0.0.1:8080"

post(url, data="test", headers={"Authorization": "Bearer test"})
post(url, json={"test": "test"})
get(url)
get(url, params={"test": "test"})
