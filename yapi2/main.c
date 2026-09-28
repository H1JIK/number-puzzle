#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NOT_INIT -2
#define INIT -1


typedef struct {
	char** list;
	char* signs;
	unsigned int len;
	char* used;
} smart_lst;
smart_lst before_equals;
char* after_equals;

typedef struct {
	char* letter;
	int* digit;
} char_dict;
char_dict cd;

char alph[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

unsigned int attemps;

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
char* read_lines() {
	int shift = 16;
	char* str = malloc(shift);
	char cur_let;
	unsigned int idx = 0;
	do {
		cur_let = fgetc(stdin);
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

	//clean
	free(before_equals.list);
	free(before_equals.signs);
	free(after_equals);
	exit(0);
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

	//add in list
	for (int i = 0; i < len; i++) {
		if (before_equals.list[i][1] != '\0' && get_digit(before_equals.list[i][0]) == 0)
			return 0;
		nums[i] = numb_from_word(before_equals.list[i]);
	}

	//multiply and div
	for (int j = 0; j < len; j++) {
		if (before_equals.signs[j] == '*' || before_equals.signs[j] == '/') {
			switch (before_equals.signs[j]) {
			case '*':
				nums[j] *= nums[j + 1];
				break;
			case '/':
				nums[j] /= nums[j + 1];			//only full-div
				break;
			}
			nums = shift_on_list_left(--len, j + 1, nums);
		}
	}

	//addition and subtraction
	for (int k = 0; k < len; k++) {
		if (before_equals.signs[k] == '+' || before_equals.signs[k] == '-') {
			switch (before_equals.signs[k]) {
			case '+':
				nums[k] += nums[k + 1];
				break;
			case '-':
				nums[k] -= nums[k + 1];
				break;
			}
			nums = shift_on_list_left(--len, k + 1, nums);
		}
	}
	return nums[0];
}

int lets_test() {
	attemps++;
	before_equals.used = calloc(before_equals.len, 1);
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

void brootforce_letters(int cur_pos) {
	if (cur_pos >= strlen(cd.letter))
		return;
	for (int d = 0; d < 10; d++) {
		if (already_busy_digit(cd.letter[cur_pos], d, cur_pos))
			continue;
		set_let(cd.letter[cur_pos], d);
		brootforce_letters(cur_pos + 1);
		if (lets_test()) {
			print_stats();
		}
	}
}

void run_selection() {
	brootforce_letters(0);
}


int main() {
	//inits
	char* input = NULL;
	init_dict();
	//start
	input = read_lines();
	split_str(input);
	run_selection();



	return 0;
}