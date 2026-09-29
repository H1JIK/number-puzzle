#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



#define NOT_INIT -2
#define INIT -1


typedef struct {
	char** list;
	char* signs;
	unsigned int len;
} smart_lst;
smart_lst before_equals;
char* after_equals;

typedef struct {
	char* letter;
	int* digit;
} char_dict;
char_dict cd;

char alph[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
clock_t start_time;
unsigned int attemps;

void before_equals_init() {
	attemps = 0;
	before_equals.len = 0;
}

int is_big_alph(char c) {
	if (c >= 65 && c <= 90)
		return 1;
	return 0;
}

void init_dict() {
	cd.letter = malloc(1);
	cd.letter[0] = '\0';
	cd.digit = malloc(26 * sizeof(int));
	for (int i = 0; i < 26; i++) {
		cd.digit[i] = NOT_INIT;
	}
}

void add_let(char c) {
	int dict_len = strlen(cd.letter);
	for (int i = 0; i < dict_len; i++) {
		if (cd.letter[i] == c)
			return;
	}
	cd.letter = realloc(cd.letter, dict_len + 2);
	cd.letter[dict_len + 1] = '\0';
	cd.letter[dict_len] = c;
	cd.digit[c - 65] = INIT;
}

void set_let(char c, int d) {
	cd.digit[c - 65] = d;
}

int get_digit(char c) {
	return cd.digit[c - 65];
}

//machine-independent -> realloc (bigger shift)
char* read_lines(char mode, FILE* f) {
	int shift = 16;
	char* str = malloc(shift);
	char cur_let;
	unsigned int idx = 0;
	do {
		if (mode == '1')
			cur_let = fgetc(stdin);
		else if (mode == '2') {
			cur_let = fgetc(f);
			if (cur_let == EOF)
				return EOF;
		}
		if (is_big_alph(cur_let))
			add_let(cur_let);
		str[idx++] = cur_let;
		if (idx % shift == 0)
			str = realloc(str, idx + 1 + shift);
	}
	while (cur_let != '\n');
	str[idx] = '\0';
	str = realloc(str, (idx + 1));
	return str;	
}

void split_str(char* str) {
	unsigned int i = 0;
	unsigned int start = 0;
	unsigned int fin = 0;
	while (str[i] != '\0') {
		if (str[i] == ' ') {
			fin = i - 1;
			if (fin >= start) {
				before_equals.list = realloc(before_equals.list, (before_equals.len + 1) * sizeof(char*));
				before_equals.list[before_equals.len] = malloc((fin - start + 2) * sizeof(char));
				before_equals.signs = realloc(before_equals.signs, (before_equals.len + 1));	//type char = 1 byte
				before_equals.signs[before_equals.len] = str[i + 1];
				for (int j = start; j <= fin; j++) {
					before_equals.list[before_equals.len][j - start] = str[j];
				}
				before_equals.list[before_equals.len++][fin - start + 1] = '\0';
				i += 3;	//space sign space word
				start = i;
			}
		}
		if (str[i] == '\n') {
			fin = i - 1;
			after_equals = malloc(fin - start + 2);
			for (int j = start; j <= fin; j++) {
				after_equals[j - start] = str[j];
			}
			after_equals[fin - start + 1] = '\0';
			return;
		}
	i++;
	}
}

int numb_from_word(char* word) {
	int len = strlen(word);
	char* digits_lst = malloc(len + 1);
	for (int i = 0; i < len; i++) {
		digits_lst[i] = (char)(get_digit(word[i]) + 48);
	}
	digits_lst[len] = '\0';
	int numb = atoi(digits_lst);
	free(digits_lst);
	return numb;
}

void print_stats() {
	printf("ATTEMPS: %d\n", attemps);

	for (int i = 0; i < before_equals.len; i++) {
		printf("%s %c ", before_equals.list[i], before_equals.signs[i]);
	}
	printf("%s\n", after_equals);

	for (int i = 0; i < before_equals.len; i++) {
		printf("%d %c ", numb_from_word(before_equals.list[i]), before_equals.signs[i]);
		free(before_equals.list[i]);
	}
	printf("%d\n", numb_from_word(after_equals));
	clock_t end = clock();
	double seconds = (double)(end - start_time) / CLOCKS_PER_SEC;
	printf("TIME: %f seconds\n", seconds);
	printf("---------------------------------\n");


	//exit(0);
}

void print_table() {
	printf("\n");

	for (int i = 0; i < 26; i++) {
		printf("%-4c", alph[i]);
	}
	printf("\n");
	for (int i = 0; i < 26; i++) {
		printf("%-4d", cd.digit[i]);
	}
	printf("\nActive letters: ");
	for (int k = 0; k < strlen(cd.letter); k++) {
		printf("%c ", cd.letter[k]);
	}
}

int* shift_on_list_left(int len, int delete_ind, int* nums) {
	if (delete_ind > 0) {
		for (int i = delete_ind; i < len; i++) {
			nums[i] = nums[i + 1];
		}
	}
	nums = realloc(nums, sizeof(int) * len);
	return nums;
}

int calculate() {
	int len = before_equals.len;
	int* nums = malloc(len * sizeof(int));
	int cur_operand = 0;

	//set_let('A', 2);
	//set_let('B', 5);
	//set_let('C', 6);
	//set_let('D', 3);
	//set_let('E', 4);
	//set_let('F', 7);

	//add in list
	for (int i = 0; i < len; i++) {
		if (before_equals.list[i][1] != '\0' && get_digit(before_equals.list[i][0]) == 0)
			return 0;
		nums[i] = numb_from_word(before_equals.list[i]);
	}

	

	int numb_shift = 0;
	int elem_id;
	//multiply and div
	for (int j = 0; j < len; j++) {
		if (before_equals.signs[j] == '*' || before_equals.signs[j] == '/') {
			elem_id = j - numb_shift;
			switch (before_equals.signs[j]) {
			case '*':
				nums[elem_id] *= nums[elem_id + 1];
				break;
			case '/':
				if ((nums[elem_id + 1] == 0) || (nums[elem_id] % nums[elem_id + 1] != 0)) {
					attemps--;
					return 0;
				}
				nums[elem_id] /= nums[elem_id + 1];			//only full-div
				break;
			}
			nums = shift_on_list_left(--len, elem_id + 1, nums);
			numb_shift++;
		}
	}
	
	numb_shift = 0;
	//addition and subtraction
	for (int k = 0; len != 1; k++) {
		elem_id = k - numb_shift + 1;
		if (before_equals.signs[k] == '+' || before_equals.signs[k] == '-') {
			len--;
			switch (before_equals.signs[k]) {
			case '+':
				nums[0] += nums[elem_id];
					break;
			case '-':
				nums[0] -= nums[elem_id];
				break;
			}
		}
		else {
			numb_shift++;
		}
	}
	int res = nums[0];
	free(nums);
	return res;
}

int lets_test() {
	if (before_equals.len == 2) {
		if (strlen(before_equals.list[0]) == strlen(before_equals.list[1])) {
			if (get_digit(before_equals.list[0][0]) + get_digit(before_equals.list[1][0]) < 9)
				return 0;
		}
	}
	attemps++;
	int sum = 0;
	if (after_equals[1] != '\0' && get_digit(after_equals[0]) == 0) {		//replace on [1] != '\0'
		return 0;
	}
	int result = numb_from_word(after_equals);
	sum = calculate();
	return sum == result;
}

int already_busy_digit(char c, int d, int pos) {
	for (int i = 0; i < pos; i++) {
		if (d == get_digit(cd.letter[i]) && cd.letter[i] != c)
			return 1;
	}
	return 0;
}

int brootforce_letters(int cur_pos) {
	if (cur_pos >= strlen(cd.letter))
		return 0;
	for (int d = 0; d < 10; d++) {
		if (already_busy_digit(cd.letter[cur_pos], d, cur_pos))
			continue;
		set_let(cd.letter[cur_pos], d);
		if (brootforce_letters(cur_pos + 1) == 1)
			return 1;
		if (cur_pos == (strlen(cd.letter) - 1) && lets_test()) {
			print_stats();
			return 1;
		}
	}
}

void run_selection() {
	brootforce_letters(0);
}

char input_mode() {
	char mode;
	printf("\tModes:\n1-Input\n2-File\nChoose the mode: ");
	scanf("%c", &mode);
	while (getchar() != '\n');
	if (mode != '1' && mode != '2') {
		printf("ERROR. The mode is incorrect.\n\n\n");
		return input_mode();
	}
	return mode;
}

int main() {
	//inits
	char* input = NULL;
	init_dict();
	char path[1024];
	char mode = input_mode();

	switch (mode) {
	case '1':
		printf("Input the string: ");
		input = read_lines(mode, NULL);
		split_str(input);
		start_time = clock();
		run_selection();
		break;
	case '2':
		printf("Input the path of file: ");
		scanf("%s", path);
		FILE* f = fopen(path, "r");
		while (1) {
			input = read_lines(mode, f);
			if (input == EOF)
				exit(0);
			split_str(input);
			start_time = clock();
			run_selection();
			free(input);
			init_dict();
			before_equals_init();
		}
		break;
	}

	//clean
	free(before_equals.list);
	free(before_equals.signs);
	free(after_equals);
	return 0;
}