FROM php:7.4-cli

COPY bolt.so /usr/local/lib/php/extensions/no-debug-non-zts-20190902/
COPY bolt.ini /usr/local/etc/php/conf.d/

COPY chall.php /app/

CMD [ "php", "/app/chall.php" ]