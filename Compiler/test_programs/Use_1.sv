@use test_programs\Use_2.sv;

void main() {
	int x = 5;
	int y = 10;
	
	int c = sum(x, y);
	
	if(c > 50) {
		c = c + 1;
	}
}
