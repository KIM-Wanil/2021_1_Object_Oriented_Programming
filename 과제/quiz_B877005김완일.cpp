#include <iostream>

// quiz1 - 실행 성공
int countMatchedNumberLetterInString(const char* str, int digit)
{
	int stringLength = strlen(str);//문자열:"112233443311334913201234" ->stringLength:24
	int count = 0;
	for (int i = 0;i < stringLength;i++)
	{
		if (str[i]-'0' == digit)//str[i]는 '2','4'와 같은 문자이고 digit은 int형 숫자이므로
			count++;
	}
	return count;
}

// quiz2 - 실행 성공
void excludeRedundantLetters(char* str)
{
	int stringLength = strlen(str);//문자열:"Hello World" ->stringLength:11
	for (int i = 0;i < stringLength;i++)
	{
		for (int j = 0;j < i;j++)//str[i]가 str[0]~str[i-1]까지 겹치는지 확인
		{
			if (str[i] == str[j])
			{
				for (int k = i;k < stringLength;k++)//겹치는 경우 str[i+1]부터 한칸씩 왼쪽으로 옮김
					str[k] = str[k + 1];
				stringLength--;
				i--;
				break;
			}
		}
	}
}

// quiz3 - 실행 성공
int countWords(const char* str)
{
	int stringLength = strlen(str);//문자열:"Hello World. Here	is the train\n"->stringLength:31
	int count = 0;
	for (int i = 0;i < stringLength;i++)
	{
		if (count < 1)//단어의 개수가 0일수 있기 때문에 count가 0일 때는 단어 후에 공백이 나왔을때 카운트함.
		{
			if ((str[i] != ' ' && str[i] != '\t') && (str[i + 1] == ' ' || str[i + 1] == '\t'))
				count++;
		}
		else//단어가 1개이상인 경우 위와 같은 방식으로 카운트하면 문자열 끝에 공백이 나왔을 때 단어 개수가 늘어나게 되므로 공백 후에 단어가 나왔을 때 카운트함. 
		{
			if ((str[i] == ' ' || str[i] == '\t') && (str[i + 1] != ' ' && str[i + 1] != '\t' && str[i + 1] != 0))
				count++;
		}
	}
	return count;
}


// quiz4 - 실행 성공
bool examinePairedParenthesis(const char* str)
{
	int stringLength = strlen(str);//문자열:"((()))"->stringLength:6
	int openParen = 0;
	int closeParen = 0;
	if (str[0] == ')' || str[stringLength - 1] == '(')//닫는 소괄호로 시작하거나 여는 소괄호로 끝나면 바로 false 반환
		return false;
	else
	{
		for (int i = 0;i < stringLength;i++)
		{
			if (str[i] == '(')
				openParen++;
			else if (str[i] == ')')
				closeParen++;
		}
		//닫는 소괄호와 여는 소괄호의 개수 측정
	}
	if (openParen == closeParen)
		return true;
	else
		return false;
	//닫는 소괄호의 개수와 여는 소괄호의 개수가 같으면 true 반환 아니면 false 반환
}

// quiz5 - 실행 성공
void convertString(char* str)
{
	int stringLength = strlen(str);//문자열:"Hello World. Nice to Meet  you"-> stringLength:32
	int spaceNum = 0;
	for (int i = 0;i < stringLength;i++)
	{
		if (str[i] == ' ')
			spaceNum++;
	}//공백문자의 개수를 구함, spaceNum=7;
	char* spaceString=(char*)malloc(sizeof(char)*(spaceNum+1));//공백문자 개수 크기의 문자열 생성
	int spaceIndex = 0;
	for (int i = 0;i < stringLength;i++)
	{
		if (str[i] == ' ')
			spaceString[spaceIndex++] = i;
	}//spaceString 문자열에 str문자열의 공백문자가 있는 인덱스를 차례로 대입함
	spaceString[spaceNum] = 0;
	for (int i = stringLength;i >= 0;i--)
	{
		int moveCount = 0;
		for (int j = 0;j < spaceNum;j++)
		{
			if (i >= spaceString[j])
				moveCount++;
		}//str문자열 뒤부터 차례대로 문자 앞에 공백문자가 몇 개 존재하는 지 카운트함
		if (str[i] == ' ')
		{
			str[i + (moveCount * 2)] = '0';
			str[i + (moveCount * 2) - 1] = '2';
			str[i + (moveCount * 2) - 2] = '%';
		}//공백문자의 경우 앞의 공백문자*2 만큼 인덱스를 옮기고 %,2,0을 차례대로 대입함
		else
		{
			str[i + (moveCount * 2)] = str[i];
		}//공백문자가 아닌 경우 앞의 공백문자*2만큼 인덱스를 옮김
	}
	free(spaceString);

}




int main()
{
	int count = countMatchedNumberLetterInString("112233443311334913201234", 4);
	printf("quiz1: %d\n", count);

	char data[100] = { "Hello World" };
	excludeRedundantLetters(data);
	printf("quiz2: %s\n", data);

   	int nWords = countWords("Hello World. Here	is the train\n");
	printf("quiz3: %d\n", nWords);

	bool result = examinePairedParenthesis("((()))");
	printf("quiz4: %s\n", result == true ? "true" : "false");

	char input[100] = "Hello World. Nice to Meet   you.";
	convertString(input);
	printf("quiz5: %s\n", input);

	return 0;
}
