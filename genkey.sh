# openssl req -new -newkey rsa:1024 -nodes -out ca.csr -keyout ca.key
# openssl x509 -trustout -signkey ca.key -days 365 -req -in ca.csr -out ca.pem
#
# openssl genrsa -out client.key 1024
# openssl req -new -key client.key -out client.csr
# openssl ca -in client.csr -out client.cer
#
# Interactive
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -sha256 -days 365
