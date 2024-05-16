import os
import random


# 生成 1024 个随机浮点数，可以设置随机数的范围，将结果保存到 input.txt 文件中

def gen(n, min_value, max_value, filepath):
    with open(filepath, 'w') as f:
        for _ in range(n):
            f.write(str(random.uniform(min_value, max_value)) + '\n')


if __name__ == '__main__':
    # 42 是随机数种子，保证每次生成的随机数都是一样的
    random.seed(42)
    gen(1024, 1e-6, 1e-10, 'input.txt')
    print('Done')
