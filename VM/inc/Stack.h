#pragma once

#include "LinkedList.h"

namespace jci {
	template<typename T>
	class Stack {
	private:
		LinkedList<T> stack;
		int maxSize, length;
	public:
		//Constructors
		Stack() {maxSize = -1; length = 0;};
		Stack(int maxLength) {maxSize = maxLength; length = 0;};

		//Sets the max length of the stack to the new max length
		void setMaxLength(int newMaxLength) {
			stack.clear();
			maxSize = newMaxLength;
		}

		//Get the current length of the stack
		int getLength() {return length;}

		//Push new element to top of the stack
		//Param element -> element to be pushed
		//Returns true if successful or false in case of failure (max length of the stack reached)
		bool push(T element) {
			if(maxSize == -1) {
				stack.addElementFront(element);
				length++;
				return true;
			}else if(maxSize - length > 0) {
				stack.addElementFront(element);
				length++;
				return true;
			}else{
				return false;
			}
		};

		//Pop an element from the top of the stack
		//Returns true if successful or false in case of failure (no elements to pop)
		bool pop() {
			if(length > 0) {
				stack.removeElementFront();
				length--;
				return true;
			}else{
				return false;
			}
		};

		//Get the value from the top element of the stack
		//Returns T, the value
		T peek() {return stack.getElementFront()->content;};

		//Check if the stack is full
		//Returns true if the stack has not reached its max size, false otherwise
		bool isFull() {
			if(maxSize == -1) {
				return false;
			}else if(maxSize - length > 0) {
				return false;
			}else{
				return true;
			}
		};

		//Check if the stack is empty
		//Returns true if there are no elements in the stack, false otherwise
		bool isEmpty() {
			if(length == 0) {
				return true;
			}else{
				return false;
			}
		};
	};
}