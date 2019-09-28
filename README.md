#SVML

SVML is a hobby project mainly being made for fun and education, still very WIP. It is not meant to be a competitive language but the developement is making it to furthest of its capabilities. It has C-like syntax currently but that is subject to change. It is a VM based programming language and currently the compiler and the VM are separate.

Currently it supports basic language capabilities:
-If statements
-While loops
-Function definitions
-Function calls
-Multiple file support

Some example programs:

-Simple addition
```
void main() {
	int x = 5;
	int y = 10;

	int z = x + y;
}
```

-Function definition and if statement
```
int sum(int a, int b) {
	int c = a+b;
	return c;
}

void main() {
	int x = 5;
	int y = 10;
	
	int c = sum(x, y);
	
	if(c > 2) {
		c = c + 1;
	}
}
```

-Function defintion in two separate files
File_1:
```
@use File_2.txt;

void main() {
	int x = 5;
	int y = 10;
	
	int c = sum(x, y);
}
```
File_2:
```
int sum(int a, int b) {
	int c = a + b;
	return c;
}
```