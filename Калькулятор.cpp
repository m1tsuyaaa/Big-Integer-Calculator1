#include <iostream>
#include <string>
using namespace std;

const int MAX_DIGITS = 1000;
int firstNumberDigits[MAX_DIGITS], firstNumberLength, firstNumberSign;
int secondNumberDigits[MAX_DIGITS], secondNumberLength, secondNumberSign;
int resultDigits[MAX_DIGITS], resultLength, resultSign;

void removeLeadingZeros(int digitsArray[], int& digitsCount, int& numberSign) {
    while (digitsCount > 1 && digitsArray[--digitsCount] == 0);
    ++digitsCount;
    if (digitsCount == 1 && digitsArray[0] == 0) numberSign = 0;
}

int compareAbsolute(const int firstArray[], int firstLength, const int secondArray[], int secondLength) {
    if (firstLength != secondLength) return firstLength > secondLength ? 1 : -1;
    for (int index = firstLength - 1; index >= 0; --index)
        if (firstArray[index] != secondArray[index]) return firstArray[index] > secondArray[index] ? 1 : -1;
    return 0;
}

void initializeNumber(int digitsArray[], int& digitsCount, int& numberSign, const string& inputString = "") {
    digitsCount = 1;
    digitsArray[0] = 0;
    numberSign = 0;
    if (inputString.empty()) return;
    
    int startIndex = 0;
    if (inputString[0] == '-') {
        numberSign = 1;
        ++startIndex;
    } else if (inputString[0] == '+') {
        ++startIndex;
    }
    
    digitsCount = 0;
    for (int position = inputString.length() - 1; position >= startIndex; --position) {
        if (isdigit(inputString[position]) && digitsCount < MAX_DIGITS)
            digitsArray[digitsCount++] = inputString[position] - '0';
    }
    removeLeadingZeros(digitsArray, digitsCount, numberSign);
}

void copyNumber(const int sourceArray[], int sourceLength, int sourceSign, int destinationArray[], int& destinationLength, int& destinationSign) {
    destinationLength = sourceLength;
    destinationSign = sourceSign;
    for (int index = 0; index < sourceLength; ++index)
        destinationArray[index] = sourceArray[index];
}

void addNumbers(const int firstArray[], int firstLength, int firstSign, const int secondArray[], int secondLength, int secondSign, int resultArray[], int& resultLength, int& resultSign);

void subtractNumbers(const int firstArray[], int firstLength, int firstSign, const int secondArray[], int secondLength, int secondSign, int resultArray[], int& resultLength, int& resultSign) {
    if (firstSign != secondSign) {
        int temporaryArray[MAX_DIGITS], temporaryLength, temporarySign;
        copyNumber(secondArray, secondLength, secondSign, temporaryArray, temporaryLength, temporarySign);
        temporarySign = temporarySign ? 0 : 1;
        addNumbers(firstArray, firstLength, firstSign, temporaryArray, temporaryLength, temporarySign, resultArray, resultLength, resultSign);
        return;
    }

    int comparison = compareAbsolute(firstArray, firstLength, secondArray, secondLength);
    if (comparison < 0) {
        subtractNumbers(secondArray, secondLength, secondSign, firstArray, firstLength, firstSign, resultArray, resultLength, resultSign);
        resultSign = firstSign ? 0 : 1;
        return;
    }

    resultSign = firstSign;
    resultLength = firstLength;
    int borrowValue = 0;
    for (int index = 0; index < firstLength; ++index) {
        int difference = firstArray[index] - borrowValue;
        if (index < secondLength) difference -= secondArray[index];
        if (difference < 0) {
            difference += 10;
            borrowValue = 1;
        } else {
            borrowValue = 0;
        }
        resultArray[index] = difference;
    }
    removeLeadingZeros(resultArray, resultLength, resultSign);
}

void addNumbers(const int firstArray[], int firstLength, int firstSign, const int secondArray[], int secondLength, int secondSign, int resultArray[], int& resultLength, int& resultSign) {
    if (firstSign != secondSign) {
        int temporaryArray[MAX_DIGITS], temporaryLength, temporarySign;
        copyNumber(secondArray, secondLength, secondSign, temporaryArray, temporaryLength, temporarySign);
        temporarySign = temporarySign ? 0 : 1;
        subtractNumbers(firstArray, firstLength, firstSign, temporaryArray, temporaryLength, temporarySign, resultArray, resultLength, resultSign);
        return;
    }

    resultSign = firstSign;
    resultLength = 0;
    int maxLength = firstLength > secondLength ? firstLength : secondLength;
    int carryValue = 0;
    for (int index = 0; index < maxLength || carryValue; ++index) {
        int sum = carryValue;
        if (index < firstLength) sum += firstArray[index];
        if (index < secondLength) sum += secondArray[index];
        if (resultLength < MAX_DIGITS) resultArray[resultLength++] = sum % 10;
        carryValue = sum / 10;
    }
    int tempSign = resultSign;
    removeLeadingZeros(resultArray, resultLength, resultSign);
    resultSign = tempSign;
}

void multiplyNumbers(const int firstArray[], int firstLength, int firstSign, const int secondArray[], int secondLength, int secondSign, int resultArray[], int& resultLength, int& resultSign) {
    for (int index = 0; index < MAX_DIGITS; ++index) resultArray[index] = 0;
    resultLength = 1;
    resultSign = 0;
    for (int firstIndex = 0; firstIndex < firstLength; ++firstIndex) {
        int carryValue = 0;
        for (int secondIndex = 0; secondIndex < secondLength || carryValue; ++secondIndex) {
            long long product = resultArray[firstIndex + secondIndex] + firstArray[firstIndex] * (secondIndex < secondLength ? secondArray[secondIndex] : 0) + carryValue;
            if (firstIndex + secondIndex < MAX_DIGITS) resultArray[firstIndex + secondIndex] = product % 10;
            carryValue = product / 10;
        }
    }
    resultLength = firstLength + secondLength;
    if (resultLength > MAX_DIGITS) resultLength = MAX_DIGITS;
    resultSign = firstSign != secondSign ? 1 : 0;
    removeLeadingZeros(resultArray, resultLength, resultSign);
}

void divideNumbers(const int firstArray[], int firstLength, int firstSign, const int secondArray[], int secondLength, int secondSign, int resultArray[], int& resultLength, int& resultSign) {
    if (secondLength == 1 && secondArray[0] == 0) throw runtime_error("Division by zero");
    int tempDividend[MAX_DIGITS], tempDividendLength, tempDividendSign;
    int tempDivisor[MAX_DIGITS], tempDivisorLength, tempDivisorSign;
    copyNumber(firstArray, firstLength, firstSign, tempDividend, tempDividendLength, tempDividendSign);
    copyNumber(secondArray, secondLength, secondSign, tempDivisor, tempDivisorLength, tempDivisorSign);
    tempDividendSign = 0;
    tempDivisorSign = 0;
    if (compareAbsolute(tempDividend, tempDividendLength, tempDivisor, tempDivisorLength) < 0) {
        initializeNumber(resultArray, resultLength, resultSign);
        return;
    }
    for (int index = 0; index < MAX_DIGITS; ++index) resultArray[index] = 0;
    resultLength = firstLength;
    resultSign = firstSign != secondSign ? 1 : 0;
    int currentValue[MAX_DIGITS], currentValueLength, currentValueSign;
    initializeNumber(currentValue, currentValueLength, currentValueSign);
    for (int position = firstLength - 1; position >= 0; --position) {
        for (int shiftIndex = currentValueLength; shiftIndex > 0; --shiftIndex)
            currentValue[shiftIndex] = currentValue[shiftIndex - 1];
        currentValue[0] = firstArray[position];
        ++currentValueLength;
        removeLeadingZeros(currentValue, currentValueLength, currentValueSign);
        int quotientDigit = 0;
        while (compareAbsolute(currentValue, currentValueLength, tempDivisor, tempDivisorLength) >= 0) {
            subtractNumbers(currentValue, currentValueLength, currentValueSign, tempDivisor, tempDivisorLength, tempDivisorSign, currentValue, currentValueLength, currentValueSign);
            ++quotientDigit;
        }
        resultArray[position] = quotientDigit;
    }
    removeLeadingZeros(resultArray, resultLength, resultSign);
}

int numbersEqual(const int firstArray[], int firstLength, int firstSign, const int secondArray[], int secondLength, int secondSign) {
    if (firstSign != secondSign || firstLength != secondLength) return 0;
    for (int index = 0; index < firstLength; ++index)
        if (firstArray[index] != secondArray[index]) return 0;
    return 1;
}

int firstLessThanSecond(const int firstArray[], int firstLength, int firstSign, const int secondArray[], int secondLength, int secondSign) {
    if (firstSign != secondSign) return firstSign;
    int comparison = compareAbsolute(firstArray, firstLength, secondArray, secondLength);
    return firstSign ? comparison > 0 : comparison < 0;
}

string numberToString(const int digitsArray[], int digitsCount, int numberSign) {
    if (digitsCount == 0) return "0";
    string resultString;
    if (numberSign) resultString += '-';
    for (int position = digitsCount - 1; position >= 0; --position)
        resultString += to_string(digitsArray[position]);
    return resultString;
}

void inputNumber(int digitsArray[], int& digitsCount, int& numberSign, const string& prompt) {
    string inputString;
    cout << prompt;
    cin >> inputString;
    initializeNumber(digitsArray, digitsCount, numberSign, inputString);
}

void showResult(const string& operation, const int digitsArray[], int digitsCount, int numberSign) {
    cout << operation << ": " << numberToString(digitsArray, digitsCount, numberSign) << endl;
}

void showComparison(const string& operation, int result) {
    cout << operation << ": " << (result ? "true" : "false") << endl;
}

int main() {
    cout << "Big Integer Calculator" << endl;
    inputNumber(firstNumberDigits, firstNumberLength, firstNumberSign, "First number: ");
    inputNumber(secondNumberDigits, secondNumberLength, secondNumberSign, "Second number: ");
    addNumbers(firstNumberDigits, firstNumberLength, firstNumberSign, secondNumberDigits, secondNumberLength, secondNumberSign, resultDigits, resultLength, resultSign);
    showResult("Sum", resultDigits, resultLength, resultSign);
    subtractNumbers(firstNumberDigits, firstNumberLength, firstNumberSign, secondNumberDigits, secondNumberLength, secondNumberSign, resultDigits, resultLength, resultSign);
    showResult("Difference", resultDigits, resultLength, resultSign);
    multiplyNumbers(firstNumberDigits, firstNumberLength, firstNumberSign, secondNumberDigits, secondNumberLength, secondNumberSign, resultDigits, resultLength, resultSign);
    showResult("Product", resultDigits, resultLength, resultSign);
    divideNumbers(firstNumberDigits, firstNumberLength, firstNumberSign, secondNumberDigits, secondNumberLength, secondNumberSign, resultDigits, resultLength, resultSign);
    showResult("Quotient", resultDigits, resultLength, resultSign);
    cout << "Comparisons:" << endl;
    showComparison("Equal", numbersEqual(firstNumberDigits, firstNumberLength, firstNumberSign, secondNumberDigits, secondNumberLength, secondNumberSign));
    showComparison("Not equal", !numbersEqual(firstNumberDigits, firstNumberLength, firstNumberSign, secondNumberDigits, secondNumberLength, secondNumberSign));
    showComparison("First < Second", firstLessThanSecond(firstNumberDigits, firstNumberLength, firstNumberSign, secondNumberDigits, secondNumberLength, secondNumberSign));
    showComparison("First > Second", firstLessThanSecond(secondNumberDigits, secondNumberLength, secondNumberSign, firstNumberDigits, firstNumberLength, firstNumberSign));
    showComparison("First <= Second", !firstLessThanSecond(secondNumberDigits, secondNumberLength, secondNumberSign, firstNumberDigits, firstNumberLength, firstNumberSign));
    showComparison("First >= Second", !firstLessThanSecond(firstNumberDigits, firstNumberLength, firstNumberSign, secondNumberDigits, secondNumberLength, secondNumberSign));
    return 0;
}