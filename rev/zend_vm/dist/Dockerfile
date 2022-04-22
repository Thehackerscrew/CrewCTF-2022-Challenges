FROM php:7.4.28-cli
RUN docker-php-ext-install opcache
RUN mkdir -p /app/cache

COPY chall.php /app/
COPY chall.php.bin /app/cache/a0b003f54b6a678de9cdedfbb93ab86d/app/chall.php.bin

CMD php -d opcache.enable_cli=1 -d opcache.file_cache_only=1 -d opcache.file_cache=/app/cache -d opcache.validate_timestamps=0 /app/chall.php