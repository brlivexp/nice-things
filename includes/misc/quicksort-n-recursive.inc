stock QuickSort(numbers[], size = sizeof(numbers)) {
	#define PUSH(%1,%2)	     (stack[top][0] = (%1), stack[top][1] = (%2), top++)
	#define	POP(%1,%2)	     (--top, (%1 = stack[top][0]), (%2 = stack[top][1]))
	
	static stack[256][2];
	new top = 0;
	
	if (size <= 1)
		return;
	
	new left, right, pivot, l_hold, r_hold;
	
	PUSH(0, size - 1);
	
	while (0 < top) {
		POP(left, right);
		
		pivot = numbers[left];
		l_hold = left;
		r_hold = right;

		// insertion sort for small fragments
		if (right - left <= 6) {
			insertion_sort(numbers, left, right);

			continue;
		}

		while (left < right) {
			while (numbers[right] >= pivot && left < right)
				right--;

			if (left != right) {
				numbers[left] = numbers[right];
				left++;
			}

			while (numbers[left] <= pivot && left < right)
				left++;

			if (left != right) {
				numbers[right] = numbers[left];
				right--;
			}
		}

		numbers[left] = pivot;
		pivot = left;
		left = l_hold;
		right = r_hold;
		
		if (left < pivot)  PUSH(left, pivot - 1);
		if (right > pivot) PUSH(pivot + 1, right);
	}
	
	#undef PUSH
	#undef POP
}

stock insertion_sort(array[], left, right) {
	for (new j = left + 1; j <= right; j++) {
		new key = array[j], i = j - 1;
		
		while (i >= left && array[i] > key) {
			array[i + 1] = array[i];
			
			i--;
		}
		
		array[i + 1] = key;
	}
}