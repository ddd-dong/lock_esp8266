from rfid_wifi.db import DB
import requests
from pymongo import MongoClient
import datetime
import time


openfoor=False
esp_ip = "192.168.105.42"
url = "mongodb+srv://tst:D1S6yHVKzPW6Sng3@cluster0.wbztfhw.mongodb.net/?retryWrites=true&w=majority"
golbal_permission_list = ("admin","user","guest")

card_db = DB(url)

print("start")
while True:
    try:
        response = requests.get(f'http://{esp_ip}/test', params={'message': openfoor})
        rfid_data = card_db.load_user(response.text)
        if rfid_data:
            print(rfid_data)
            openfoor=True
            requests.get(f'http://{esp_ip}/test', params={'message': openfoor})
            openfoor=False
        else:
            if response.text !="": print(response.text)
            requests.get(f'http://{esp_ip}/test', params={'message': False})
    except Exception as erro:
        print(erro)
    time.sleep(1)
