#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>

std::mt19937 gen(42);
const int threshold = 20;

// Сортировка вставками для индексов [left, right]
void insertionSort(std::vector<int> &v, int left, int right) {
  for (int i = left + 1; i <= right; ++i) {
    int key = v[i];
    int j = i - 1;
    while (j >= left && v[j] > key) {
      v[j + 1] = v[j];
      --j;
    }
    v[j + 1] = key;
  }
}

void merge(std::vector<int> &v, int left, int mid, int right) {
  std::vector<int> result(right - left + 1);
  int result_index = 0;
  int i = left;
  int j = mid + 1;
  while (i <= mid && j <= right) {
    if (v[i] <= v[j]) {
      result[result_index] = v[i];
      ++result_index;
      ++i;
    } else {
      result[result_index] = v[j];
      ++result_index;
      ++j;
    }
  }
  while (i <= mid) {
    result[result_index] = v[i];
    ++result_index;
    ++i;
  }
  while (j <= right) {
    result[result_index] = v[j];
    ++result_index;
    ++j;
  }
  for (int copy_i = left; copy_i <= right; ++copy_i) {
    v[copy_i] = result[copy_i - left];
  }
}

// Сортировка слиянием для индексов [left, right]
void mergeSort(std::vector<int> &v, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    mergeSort(v, left, mid);
    mergeSort(v, mid + 1, right);

    merge(v, left, mid, right);
  }
}

// Сортировка слиянием с использованием сортировки вставками для индексов [left, right]
void megrePlusInsertionSort(std::vector<int> &v, int left, int right) {
  if (right + 1 - left <= threshold) {
    insertionSort(v, left, right);
  } else {
    int mid = left + (right - left) / 2;

    megrePlusInsertionSort(v, left, mid);
    megrePlusInsertionSort(v, mid + 1, right);

    merge(v, left, mid, right);
  }
}

class ArrayGenerator {
 public:
  // Создает массив из n случайных чисел в диапазоне [a, b]
  static std::vector<int> generateRandomArray(int n, int a, int b) {
    std::vector<int> v(n);
    std::uniform_int_distribution<int> dist(a, b);
    for (int i = 0; i < n; ++i) {
      v[i] = dist(gen);
    }
    return v;
  }

  // Создает отсортированный по невозрастанию массив из n случайных чисел в диапазоне [a, b]
  static std::vector<int> generateRandomSortedArray(int n, int a, int b) {
    std::vector<int> v = generateRandomArray(n, a, b);
    std::sort(v.begin(), v.end(), std::greater<>());
    return v;
  }

  // Создает почти отсортированный по неубыванию массив из n случайных чисел в диапазоне [a, b]
  static std::vector<int> generateAlmostSortedArray(int n, int a, int b) {
    int swaps = n / 500; // Количество перестановок
    std::vector<int> v = generateRandomArray(n, a, b);
    std::sort(v.begin(), v.end());
    std::uniform_int_distribution<int> dist(0, n - 1);
    for (int i = 0; i < swaps; ++i) {
      int j = dist(gen);
      int k = dist(gen);
      std::swap(v[j], v[k]);
    }
    return v;
  }
};

class SortTester {
 public:
  static void saveSortTime(const std::vector<int> &v,
                           const std::string &filename,
                           void (*sort)(std::vector<int> &, int, int)) {
    std::ofstream outfile(filename);

    for (int i = 500; i <= v.size(); i += 100) {
      std::vector<int> subarray = std::vector<int>(v.begin(), v.begin() + i);
      auto start = std::chrono::high_resolution_clock::now();
      sort(subarray, 0, i - 1);
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      size_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
      outfile << i << "\t" << nsec << std::endl;
    }

    outfile.close();
  }
};

int main() {
  std::vector<int> random_array = ArrayGenerator::generateRandomArray(10000, 0, 6000);
  std::vector<int> random_sorted_array = ArrayGenerator::generateRandomSortedArray(10000, 0, 6000);
  std::vector<int> almost_sorted_array = ArrayGenerator::generateAlmostSortedArray(10000, 0, 6000);

  SortTester::saveSortTime(random_array, "random_array_time.txt", mergeSort);
  SortTester::saveSortTime(random_sorted_array, "random_sorted_array_time.txt", mergeSort);
  SortTester::saveSortTime(almost_sorted_array, "almost_sorted_array_time.txt", mergeSort);

  SortTester::saveSortTime(random_array, "merge+insertion_random_array_time.txt", megrePlusInsertionSort);
  SortTester::saveSortTime(random_sorted_array, "merge+insertion_random_sorted_array_time.txt", megrePlusInsertionSort);
  SortTester::saveSortTime(almost_sorted_array, "merge+insertion_almost_sorted_array_time.txt", megrePlusInsertionSort);
}
