xxd -i $1 | sed -e 's/unsigned/const unsigned/' -e 's/___//'
