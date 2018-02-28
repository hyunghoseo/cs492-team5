//Check to see that the parameters are numbers and nothing else
bool isNumber(char number[])
{
    int i = 0;
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}
