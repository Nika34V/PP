import numpy as np
import sys

def extract_matrix_data(filename):
    try:
        with open(filename, 'r') as f:
            content = f.readlines()
        
        size = int(content[0].strip())
        
        matrix_data = []
        for i in range(size):
            row_vals = list(map(float, content[i+1].strip().split()))
            matrix_data.append(row_vals)
        
        return np.array(matrix_data)
    
    except FileNotFoundError:
        print(f"Ошибка: файл {filename} отсутствует в директории")
        sys.exit(1)
    except Exception as e:
        print(f"Ошибка при обработке {filename}: {e}")
        sys.exit(1)

def validate_matrix_product(file1, file2, file3, eps=1e-10):
    print("\nПРОВЕРКА КОРРЕКТНОСТИ ВЫЧИСЛЕНИЙ")
    
    M1 = extract_matrix_data(file1)
    M2 = extract_matrix_data(file2)
    M3_cpp = extract_matrix_data(file3)
    
    print(f"   Матрица 1: {M1.shape[0]}x{M1.shape[1]}")
    print(f"   Матрица 2: {M2.shape[0]}x{M2.shape[1]}")
    print(f"   Матрица 3: {M3_cpp.shape[0]}x{M3_cpp.shape[1]}")
    
    M3_numpy = np.dot(M1, M2)
    
    abs_errors = np.abs(M3_cpp - M3_numpy)
    max_dev = np.max(abs_errors)
    avg_dev = np.mean(abs_errors)
    
    print("\nРЕЗУЛЬТАТЫ АНАЛИЗА:")
    print(f"   Пиковое отклонение: {max_dev:.6e}")
    print(f"   Среднее отклонение: {avg_dev:.6e}")
    
    if max_dev < eps:
        print("\nПРОВЕРКА УСПЕШНА: результаты идентичны!")
        print(f"   (допустимая погрешность {eps})")
        return True
    else:
        print("\nПРОВЕРКА НЕ УСПЕШНА: обнаружены расхождения!")
        print(f"   (погрешность превышает {eps})")
        
        print("\nФрагменты результирующих матриц:")
        n = min(3, M1.shape[0])
        print("   C++ программа:")
        print(M3_cpp[:n, :n])
        print("   NumPy эталон:")
        print(M3_numpy[:n, :n])
        
        return False

def main():
    if len(sys.argv) != 4:
        print("ИНСТРУКЦИЯ ПО ЗАПУСКУ:")
        print(f"  {sys.argv[0]} mat1.txt mat2.txt mat3.txt")
        print("\nПараметры:")
        print("  mat1.txt - первый сомножитель")
        print("  mat2.txt - второй сомножитель")
        print("  mat3.txt - результат умножения из C++")
        sys.exit(1)
    
    path1 = sys.argv[1]
    path2 = sys.argv[2]
    path3 = sys.argv[3]
    
    verification_passed = validate_matrix_product(path1, path2, path3)

    sys.exit(0 if verification_passed else 1)

if __name__ == "__main__":
    main()