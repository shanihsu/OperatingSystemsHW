# OS_HW1_說明文件
### 學號:F74072227
### 姓名:許湘苡
### 系級:資訊111

---
## 開發環境
使用虛擬機執行，以下為虛擬機的環境
* 	OS: Ubuntu 18.04
*	CPU: Intel(R) Core(TM) i5-8250U CPU @ 1.60GHz
*	Memory: 4GB
*	Programming Language(version): C++11
---
## 程式執行時間

| 檔案大小 | 1.1GB | 3.5GB |6.2GB |
|--|--|--|--|
| 執行時間     | 5min27.954sec    | 17min6.484sec    |33min47.094sec|	

* 跑1.1GB
![](https://i.imgur.com/KgOxNHC.png)

* 跑3.5GB
![](https://i.imgur.com/0Jwa5G8.png)

* 跑6.2GB
![](https://i.imgur.com/wwnK4Fb.png)

* 檔案大小截圖
![](https://i.imgur.com/vjaJsy4.png)


---
## 程式開發與使用說明
*	程式開發:
    *	使用`two-way external merge sort` 演算法
    *	最初使用`vector`讀取`input.txt`，`vector`的大小設為1000000000個`int`，分次將檔案讀入`vector`，並使用`c++`內建`sort`函式做排序後，排序後的資料會輸出至`temp`資料夾中並生成`run1_x.txt`檔案，以達成第一次的分割。
    *	使用迭代，將上一層產生的多個`runx_x.txt`檔案，一次兩個檔案讀入，並將`buffer`切成三塊，前兩個1/3 size的buffer當作`input buffer`，分別用來放兩個檔案的內容，後1/3 size的buffer當作`output buffer`，用來放置排序後的input buffer，當output buffer oversize時，將output buffer輸出到runx_x.txt，並清空output buffer。
    *	重複上述步驟，直到`merge`後的檔案剩下一個時，即為`output.txt`。
*	使用說明:
    *	hw1.cpp
    功能:將input.txt排序後產生一個output.txt
    ``` bash=
    # Compile:
    $ g++ hw1.cpp -o hw1 -std=c++11 -lstdc++fs
    # run: 
	# Data Path須為絕對路徑(ex : ~/Desktop/os/input.txt)
	# 產生的output.txt會在跟hw1.cpp同一個資料夾
    $ ./hw1 [Data Path]
	```

    *	createsort.cpp
    功能:建立一個亂數檔案
    ``` bash=
    # Compile
    $ g++ createsort.cpp -o createsort
    # run
    # 輸入數字-->輸入的數字代表要亂數產生的數字數量
    $ ./createsort 
    ```
---
## 效能分析報告
*   ### 對硬體效能優化程度的說明與驗證
    *	==有使用buffer v.s 沒有使用buffer==
        ++有使用buffer效能大於沒使用buffer++
    沒有使用buffer的程式，直接將檔案中的資料一個一個讀寫並排序，
    有使用buffer，將檔案中的資料使用buffer做區塊式讀寫。
比較這兩種版本，明顯發現，`有使用buffer的程式比沒有使用buffer直接讀寫的程式在跑1GB的檔案時快了十倍左右`。
這個結果是可想而知的，因為沒有使用buffer的程式在執行時，是藉由不斷開關檔案，輸入輸出檔案來完成，而讀取檔案需要從硬碟取資料，相較於此，使用buffer的程式因為已經將資料使用buffer暫存在記憶體中，只需要從記憶體中將資料取出即可。
由此驗證，`從記憶體直接讀寫資料，比從硬碟讀取資料快很多`。
    * ==使用array v.s 使用vector==
        ++使用array效能大於vector++
        vector在push_back的時候會將記憶體重新分配，而array因為一開始就將大小宣告固定，不須再花時間重新配置記憶體，所以array的執行效率較高。
    * ==分割後檔案大小越大效能越好==
        將input.txt分割成多個檔案sort，再將多個sort後的檔案merge，當分割後的檔案大小越大，分割後的檔案數量越小，需要做merge的檔案越少，可減少執行時間，提升效能。
    ---
*	### 同時運行多支排序程式的說明
    :::info
    * 同時執行兩個相同檔案hw1 hw1_1排序1.1GB資料的過程
    ![](https://i.imgur.com/fAeJVmk.png)

        * 執行hw1排序1.1GB資料結果
    ![](https://i.imgur.com/RwzibPP.png)
    
        * 執行hw1_1排序1.1GB資料結果
     ![](https://i.imgur.com/875NJsM.png)
    :::
    
    :::warning
    * 同時執行兩個相同檔案hw1 hw1_1排序3.5GB資料的過程
    ![](https://i.imgur.com/7YHOwQc.png)
    
        * 執行hw1排序3.5GB資料結果
    ![](https://i.imgur.com/2pGswNQ.png)
    
        * 執行hw1_1排序3.5GB資料結果
    ![](https://i.imgur.com/CoVtWCu.png)

    :::
    
由上述執行結果可整理出以下表格:
|      | 1.1GB | 3.5GB |
| ---- | ----- | ----- |
|   執行單一程式   |  5min27.954sec     |  17min6.484sec     |
| 同時執行兩個 | 5min59sec  |21min5sec  |
得到同時執行兩個程式會使執行時間變慢的結論，且變慢的幅度隨著檔案越大呈指數上升，推測是因為同時執行會有記憶體、CPU互相占用干擾等問題，導致CPU把可用盡資源平分給兩個程式，使得程式得到的資源不如單一執行時多。
:::danger
若一次執行四個以上程式會有其中幾個程式產生kill的狀況
==推測原因==:
執行環境的記憶體大小只有4GB，而每個排序程式所給予的記憶體大小為1GB，當四個以上的程式同時執行時，若恰好同時需要使用記憶體，則會有記憶體不夠用的情況，而導致其中一些程式自行終止。
:::


---
    
*	### 結論
下圖為同時執行兩支程式的火焰圖
![](https://i.imgur.com/Is95OZn.png)
:::success
從上圖可觀察出，當CPU使用竭盡時，會將資源平均分配在執行中的程式(hw1、hw1_1)上，以此達到善用CPU。
:::
:::warning
從上方所有觀察的結論，可做進一步優化
#### 優化
* 同時執行多個程式時，記憶體與cpu的分配應該由OS做適度調整，而非將效能完全平分於每個程式，應該由每個程式所需的記憶體與CPU多寡分配，才能使每個程式達到最大的效益。
* 當程式執行中，OS應該適度調整各程式讀寫記憶體的比例，將其達到最大效用，而非任由執行中的程式自由調用，使記憶體資源不夠，導致某些程式自行終止。
:::


