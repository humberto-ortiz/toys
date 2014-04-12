package: peek
	rm -f submit.zip
	zip submit.zip -9r *.py library --exclude *.pyc

peek:
	head *.out

sum:
	md5sum *.out > ANSWER

check:
	md5sum -c ANSWER
