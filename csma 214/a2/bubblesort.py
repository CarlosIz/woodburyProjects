def bubbleSort(arr):
    x = len(arr)

    for i in range(x-1):

        for j in range(0, x-i-1):
            if arr[j] > arr[j+1]:
                arr[j], arr[j+1] = arr[j+1], arr[j]
                for i in range(len(arr)):
                     print("Array Update")
                     print("%d" %arr[i])

arr = [1, 3, 4, 2, 5, 7, 6]

print ("Initial Array: " ) 
for i in range(len(arr)):
    print("%d" %arr[i])

bubbleSort(arr)

print ("Bubble Sort Output: ")
for i in range(len(arr)):
    print("%d" %arr[i])
