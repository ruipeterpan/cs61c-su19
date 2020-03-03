
**Midterm 2 Reivew has no official checkoffs.**

Here's some answers to the questions though.

## Exercise 1: Datapath

We didn't even put any questions in here.

## Exercise 2: Pipelining

When we pipeline a circuit, latency goes up due to the fact that we now have to
save registers in between and also we have to pipe to the longest stage.
Bandwidth goes up because now we process more instructions in the same amount
of time

## Exercise 3: Caches

```
addr bits = log_2(addr space) = log_2(4 MiB) = log_2(4 * 2^20 B) = 22 bits

offset = log_2(blocksize) = log_2(128 B) = 7 bits

#blocks = size of cache / blocksize = 2 * 2^10 B / 2^7 B = 2^4 blocks

index = log_2(#blocks / N) = log_2(2^4 Blocks/1) = 4 bits

tag = addr - offset - index = 22 - 7 - 4 = 11 bits
```

Line 1

```
Step size = 4 words = 16 B / step

Block size = 128 B / block

Steps/Block = (128 B / block) / (16 B / step) = 2^3 Steps/Block

arr[i] = 0; --> 1 access/step

--> 2^3 steps/block * 1 access/step = 8 accesses / block

Results in 7/8 hit rate for Line 1

Our cache is 2^11 B, our array is 2^12 B, meaning we have the second half of the array in the cache at the end of Line 1

Thus, for the first half of Line 2, we have no misses as it starts from the end of the array!
```

Line 2

```
Step size = 8 words = 32 B / step

Block size = 128 B / block

Steps/Block = (128 B / block) / (32 B / step) = 2^2 Steps/Block

arr[i] += i; --> 2 access/step

--> 2^2 steps/block * 2 access/step = 8 accesses / block

8/8 HR for first half of for loop, 7/8 HR for second half of for loop

--> 15/16 HR for Line 2

Overall hit rate = 1/2 * 7/8 + 1/2 * 15/16 = 29/32
```

We multiply by 1/2 because we access the same number of blocks in both lines, and each block for 8 accesses, meaning we make the same number of total accesses per line for equal weighting

Associativity would not help because we are simply iterating through the array both ways. The only way associativity might help is between Line 1 and Line 2, but it is already optimal, so no dice here

