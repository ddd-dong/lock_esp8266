import requests
esp_ip = "192.168.105.42"
requests.get(f'http://{esp_ip}/test', params={'message': True})