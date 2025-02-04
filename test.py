#!/usr/bin/env python

from requests import Session

session = Session()
session.headers.update({"Connection": "close"})  # Fermer explicitement la connexion après chaque requête

url = "http://127.0.0.1:8080"

# session.post(url, data="test", headers={"Authorization": "Bearer test"})
# session.post(url, json={"test": "test"})
session.get(url)
session.get(url, params={"test": "test"})
session.get(url, params={"test": "test", "test2": "test2"})
