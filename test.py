from requests import get

for _ in range(10):
    result = get("http://localhost:8080/api")
    print(result.text)
    print(result.status_code)
    print(result.reason)
    print(result.ok)