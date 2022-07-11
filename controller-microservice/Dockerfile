FROM php:8.1-rc-apache
# COPY src/ /var/www/html

RUN a2enmod rewrite

#RUN sed -i '/LoadModule rewrite_module/s/^#//g' /etc/apache2/conf/httpd.conf

RUN sed -i '/<Directory \/var\/www\/>/,/<\/Directory>/ s/AllowOverride None/AllowOverride All/' /etc/apache2/apache2.conf

#RUN { \
#  echo 'IncludeOptional conf.d/*.conf'; \
#} >> /etc/apache2/conf/httpd.conf \
#  && mkdir /etc/apache2/conf.d

#RUN apt-get update -y && apt-get upgrade -y;
#RUN apt-get install -y openssl libssl-dev zip libcurl4-openssl-dev git libaio1 libffi-dev

#RUN docker-php-ext-install mysqli \
#    && docker-php-ext-install sockets \
#    && docker-php-ext-configure ffi --with-ffi
#    && docker-php-ext-install ffi

#RUN pecl install mongodb && docker-php-ext-enable mongodb

#COPY --from=composer:latest /usr/bin/composer /usr/local/bin/composer
#RUN composer require mongodb/mongodb
#RUN composer require idealo/php-rdkafka-ffi

EXPOSE 80