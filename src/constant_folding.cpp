#include <iostream>

#include <stdint.h>
#include <string.h>

class Constant_folding
{
    public:

    static const int CAPACITY = 64;

    Constant_folding();
    void evaluate(const std::string &expression);
    void display();

    int64_t result;
    double fresult;

    private:

    double fstack[Constant_folding::CAPACITY];
    int64_t stack[Constant_folding::CAPACITY];
    char operators[Constant_folding::CAPACITY];


    int8_t stack_top;
    int8_t operator_top;
    int8_t stack_capacity;
    bool hexa,integer;
    std::string text;

    //---
    bool isFull();
    bool isEmpty();
    void push(int64_t item,double fitem);
    int64_t pop();

    bool isFull_operator();
    bool isEmpty_operator();
    void push_operator(char c);
    char pop_operator();
    char peek_operator();
    char precedence(char ope);

    bool isOperator(char c);
    bool isNumber(char c);
    bool isNumberDec(char c);
    bool isNumberBin(char c);
    bool isNumberHex(char c);
    int64_t applyOperation(int64_t a, int64_t b, char op);
    double fapplyOperation(double a, double b, char op);

    void apply_operation();
};

Constant_folding::Constant_folding()
{
    this->stack_top = -1;
    this->operator_top = -1;
    this->stack_capacity = Constant_folding::CAPACITY;
    this->hexa = false;
    this->result = 0;
    this->fresult = 0;
    this->text.reserve(256);
    this->integer = true;

    for(uint32_t i = 0;i < Constant_folding::CAPACITY;i++)
    {
        this->stack[i] = 0;
        this->fstack[i] = 0;
        this->operators[i] = 0;
    }
}

//-----------------------

bool Constant_folding::isFull()
{
    return this->stack_top == this->stack_capacity - 1;
}

bool Constant_folding::isEmpty()
{
    return this->stack_top == -1;
}

void Constant_folding::push(int64_t item,double fitem)
{
    if (this->isFull()) return;
    this->stack_top++;
    this->fstack[this->stack_top] = fitem;
    this->stack[this->stack_top] = item;
}

int64_t Constant_folding::pop()
{
    if(isEmpty()) return 0;
    this->fresult = this->fstack[this->stack_top];
    this->result  = this->stack[this->stack_top];
    this->stack_top--;
    return this->result;
}


//-----------------------

bool Constant_folding::isFull_operator()
{
    return this->operator_top == this->stack_capacity - 1;
}

bool Constant_folding::isEmpty_operator()
{
    return this->operator_top == -1;
}

void Constant_folding::push_operator(char item)
{
    if (this->isFull_operator()) return;
    this->operators[++this->operator_top] = item;
}

char Constant_folding::pop_operator()
{
    if(isEmpty_operator()) return 0;
    return this->operators[this->operator_top--];
}

char Constant_folding::peek_operator()
{
    if(isEmpty()) return -1;
    return this->operators[this->operator_top];
}

//-----------------------

bool Constant_folding::isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '^' || c == 1 || c == 2;
}

bool Constant_folding::isNumber(char c)
{
    if(this->hexa == false)
        return (c >= '0' && c <= '9') || (c == '.');
    else
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Constant_folding::isNumberDec(char c)
{
    return (c >= '0' && c <= '9') || (c == '.');
}

bool Constant_folding::isNumberBin(char c)
{
    return c == '0' || c == '1';
}


bool Constant_folding::isNumberHex(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

//-----------------------

char Constant_folding::precedence(char op)
{
    switch (op)
    {
        case '+':
        case '-':
            return 1;

        case 1:
        case 2:
        case '&':
        case '|':
        case '^':
            return 3;

        case '*':
        case '/':
            return 2;

        default:
            return 0;
    }
}

int64_t Constant_folding::applyOperation(int64_t a, int64_t b, char op)
{
    switch (op)
    {
        case 1:
            return a << b;
        case 2:
            return a >> b;
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        case '&':
            return a & b;
        case '|':
            return a | b;
        case '^':
            return a ^ b;
    }
    return 0;
}

double Constant_folding::fapplyOperation(double a, double b, char op)
{
    switch (op)
    {
        case 1:
            return (int64_t)a << (int64_t)b;
        case 2:
            return (int64_t)a >> (int64_t)b;
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        case '&':
            return (int64_t)a & (int64_t)b;
        case '|':
            return (int64_t)a | (int64_t)b;
        case '^':
            return (int64_t)a ^ (int64_t)b;
    }
    return 0;
}

void Constant_folding::apply_operation()
{
    int64_t b = this->pop();
    double fb = this->fresult;
    int64_t a = this->pop();
    double fa = this->fresult;
    char op = this->pop_operator();
    this->push(this->applyOperation(a, b, op),this->fapplyOperation(fa, fb, op));
}

//-----------------------

void Constant_folding::evaluate(const std::string &expression)
{
    int n = expression.size();
    char str_float[64];
    bool hexadecimal,binary;

    this->text = "";

    for (int i = 0; i < n; ++i)
    {
        char letter = expression[i];
        if ( (expression[i] == '<') && (expression[i + 1] == '<') )
            letter = 1;

        if ( (expression[i] == '>') && (expression[i + 1] == '>') )
            letter = 2;

        if ( (expression[i] == '0') && (expression[i + 1] == 'x') )
            letter = 3;

        if ( (expression[i] == '0') && (expression[i + 1] == 'b') )
            letter = 4;

        if(letter < 5)
            i++;

        this->text += letter;
    }
    this->text += " ";

    n = text.size();

    for (int i = 0; i < n; ++i)
    {
        hexadecimal = false;
        binary = false;

        if(text[i] == 3)
        {
            hexadecimal = true;
            i++;
        }

        if(text[i] == 4)
        {
            binary = true;
            i++;
        }

        this->hexa = hexadecimal;

        if (text[i] == ' ')
            continue;

        if (isNumber(text[i]) || (text[i] == '-' && isNumber(text[i + 1])))
        {
            int number = 0,l = 0;
            bool isNegative = false;
            double fnumber = 0;

            if(text[i] == '-')
            {
                isNegative = true;
                i++;
            }

            if( (binary == false) && (hexadecimal == false) )
            {
                while (i < n && isNumberDec(text[i]))
                {
                    if(text[i] == '.')
                        this->integer = false;

                    number = number * 10 + (text[i] - '0');
                    str_float[l] = text[i];
                    l++;
                    i++;
                }
                i--;
                str_float[l] = 0;

                fnumber = atof(str_float);
            }

            if(binary == true)
            {
                while (i < n && isNumberBin(text[i]))
                {
                    number = number * 2 + (text[i] - '0');
                    i++;
                }
                i--;

                fnumber = number;
            }

            if(hexadecimal == true)
            {
                while (i < n && isNumberHex(text[i]))
                {
                    char value = text[i] - '0';
                    char c = text[i];

                    if( (c >= 'a' && c <= 'f') )
                        value = text[i] - 'a' + 10;

                    if( (c >= 'A' && c <= 'F') )
                        value = text[i] - 'A' + 10;

                    number = number * 16 + value;
                    i++;
                }
                i--;

                fnumber = number;
            }

            if(isNegative == true)
            {
                number = -number;
                fnumber = -fnumber;
            }

            binary = false;
            hexadecimal = false;

            this->push(number,fnumber);
        }
        else if (text[i] == '(')
        {
            this->push_operator(text[i]);
        }
        else if (text[i] == ')')
        {
            while (!this->isEmpty_operator() && this->peek_operator() != '(')
            {
                this->apply_operation();
            }
            if (!this->isEmpty_operator() && this->peek_operator() == '(')
            {
                this->pop_operator();
            }
        }
        else if (isOperator(text[i]))
        {
            while (!this->isEmpty_operator() && this->precedence(this->peek_operator()) >= this->precedence(text[i]))
            {
                this->apply_operation();
            }
            this->push_operator(text[i]);
        }
    }

    while (!this->isEmpty_operator())
    {
        this->apply_operation();
    }

    this->pop();
}

void Constant_folding::display()
{
    std::cout << this->result << " / " << this->fresult <<std::endl;
}


//-----------------------
/*
static int test_constant()
{
    Constant_folding cfold;

    cfold.evaluate("5 + 5 + (-10*3)");
    cfold.display();

    cfold.evaluate("(-10 * 2) + (6/2))");
    cfold.display();

    cfold.evaluate("(0x10*1.5)|1");
    cfold.display();

    return 0;
}
*/

