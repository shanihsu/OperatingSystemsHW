# OS_HW2_說明文件
### 學號:F74072227
### 姓名:許湘苡
### 系級:資訊111

---
## 開發環境 :
使用虛擬機執行，以下為虛擬機的環境
* 	OS: Ubuntu 18.04
*	CPU: Intel(R) Core(TM) i5-8250U CPU @ 1.60GHz
*	Memory: 7GB
*	Programming Language(version): C++11
*	CPU:四核心
---
## 程式執行時間 :

|   Thread 數量    |    1    |   2    |   3    |   4    |   5    |   6    |   7    |   8    | 9   | 10  | 100 | 200 | 500 | 1000 |
|:----------------:|:-------:|:------:|:------:|:------:|:------:|:------:|:------:|:------:| --- | --- | --- | --- | --- | ----- |
|     I/O時間      | 10.41s | 13.53s | 13.31s | 6.72s  | 17.85s | 14.13s | 7.90s  | 11.22s  |  9.40s   |  11.19s   | 9.53s    | 8.24s    |  11.49s   |   23.43s    |
|    切data時間    |  5.81s  | 2.26s  | 1.98s  | 1.94s  | 1.84s  | 1.80s  | 1.76s  | 1.77s  |  1.75s   |  1.70s   |   1.62s  |  1.62s   |  1.67s   |   1.63s    |
| thread時間(平均) | 78.67s  | 36.87s | 26.83s | 21.50s | 22.00s | 19.98s | 19.67s | 21.03s |   20.64s  | 21.44s    | 12.32s    | 9.02s    |  4.39s   |  2.29s     |
|    總執行時間    | 96.96s  | 53.21s | 42.73s | 30.72s | 42.28s | 36.56s | 30.04s | 34.75s |  32.55s   | 35.24s    |  31.43s   |  31.66s   |  35.54s   |  47.17s     |
:::info
* 在程式執行結束時，會有四行output，括號中的顏色代表折線的顏色:
    * **I/O cost(藍色)**:讀檔與寫檔的總時間
    * **cut data cost(黃色)**:將讀進來的資料切割成指定的thread數量區塊，並存在buffer
    * **thread cost(綠色)**:平均每個thread執行所需時間
    * **total cost(紅色)**:程式執行總時間

:::
* 以下為上述數據的折線圖
![](https://i.imgur.com/Nze6rVJ.png)

* 下圖特別將thread = 1~10放大
![](https://i.imgur.com/wYAwy3a.png)

* 以下為執行時間截圖
    ![](https://i.imgur.com/SUx1dAz.png)

    ![](https://i.imgur.com/0n2ZO6K.png)
    
    ![](https://i.imgur.com/2LEt17K.png)



---
## 程式開發與使用說明 :
### 程式開發
* 使用c++內建的函式thread來完成Multi-Thread
    1. 先讀檔並計算檔案總行數後，計算每個thread平均分配到的行數，並指配分割後的內容給每個thread。
    2. 每個thread平均分配到的行數用count來存，count = 總行數/thread數量 + 1，所以最後一個thread分配到的行數會是所有thread中最小的。
    3. 用迴圈讓thread開始同步計算，thread主要功能為將被分配到的字串轉換成json格式，並將運算結果儲存於buffer陣列中
    4. 待所有thread執行完畢後，將buffer依序輸出到output.json檔案中。

###	使用說明
:::warning
**當output檔案為1GB時，任意改變thread大小均可正常執行，但當input檔案過小，且thread數量過大時，可能會造成程式異常終止。此時麻煩助教將thread數量調小，謝謝!**
:::
*	hw2.cpp
    功能:將input.csv轉換成json格式後，產生一個output.json檔
    ``` bash=
    # Compile:
    $ g++ hw2.cpp -o hw2 -lpthread
    # run: 
	# threads為可設置執行緒（thread）數量的參數
    $ ./hw2 <threads>
	```
:::warning
* 程式執行結束會有四行output，其意義標示於上述第二點程式執行時間中的藍色區塊。

:::

---
## 效能分析報告 :
### 1. 各個stage對電腦資源的使用
* 以下分成四個stage分析
    * ==**讀檔**==:讀檔耗用memory，在此階段對於CPU的需求較小，著重於memory的使用。
    * ==**將資料切割平均分配到thread**==:主要運用CPU的一個Core執行，在此階段發現，CPU的其中一個Core使用率高達9成，且會動態轉換所使用的thread編號。
    * ==**thread轉換字串**==:此階段主要是運用CPU的thread執行，使用的CPU Cores數量與thread有直接關係，第三點將會針對此部分詳談。
    * ==**寫檔**==:寫檔階段主要用memory，因為寫檔是直接將thread運算後存在buffer的結果依序寫入json檔中，在此階段CPU需求會高於讀檔，但還是著重於memory的使用，推測是因為相較於一次性讀檔，寫檔是依據thread數量依序寫入，因此需要CPU參與運算。
### 2. 各個stage對執行時間的影響:
* 下圖為上述數據的折線圖
    * **I/O cost(藍色)**:讀檔與寫檔的總時間
    * **cut data cost(黃色)**:將讀進來的資料切割成指定的thread數量區塊，並存在buffer
    * **thread cost(綠色)**:平均每個thread執行所需時間
    * **total cost(紅色)**:程式執行總時間
![](https://i.imgur.com/Nze6rVJ.png)
* 以下分成四個stage分析
    * ==**讀檔**==:在此階段因為是一次性讀入，對執行時間影響，較無太大差別。
    * ==**將資料切割平均分配到thread**==:此階段因為耗時占總執行時間比例不高，對整體執行時間影響不大。
    * ==**thread轉換字串**==:由上圖可明顯發現，在++thread數量小於CPU Cores數量時，程式總執行時間與thread呈現正相關++，第四點將會針對此部分詳談。
    * ==**寫檔**==:由上圖可發現，在當thread數量小於CPU Cores數量時，程式執行時間與寫檔時間較無關聯，但++當thread數量大於CPU Cores數量後，程式總執行時間與寫檔時間呈現正相關，且此現象在thread數量越大時越明顯++。推測是因為寫檔時需要依序將buffer中的內容寫入，當thread數量越大時，產生的資料會被分成越多區塊存在buffer，導致buffer數量越大，在依序寫入output.json檔案中，因為無法同步執行，所以增加了許多執行時間。
### 3. 不同thread對電腦資源的使用
* 以下為不同thread在電腦硬體資源的調用
:::success
* thread = 1
    ![](https://i.imgur.com/KM4krkT.png)

* thread = 2
    ![](https://i.imgur.com/wAeO8X9.png)


* thread = 3
    ![](https://i.imgur.com/D47Lzrf.png)


* thread = 4
    ![](https://i.imgur.com/vocW5Sv.png)
* thread = 10
    ![](https://i.imgur.com/MpAxcyr.png)
    
    * 由上圖可知，當程式內所調用的thread數量小於CPU Cores數量時，==CPU Cores工作滿載的數量會剛好等於程式內所調用的thread數量==。當程式內所調用的thread數量大於CPU Cores數量時，CPU會將其所有的Cores工作滿載，以達到最高效用。
    * 同時，也觀察到，在程式執行過程，==CPU Cores的使用率是動態的==，不會固定由其中幾個cores執行程式，而是隨機分配cores。就算是滿載的CPU Cores也會動態變換滿載的cores編號。
:::
:::warning
**結論:**
* 在可用範圍內，程式中thread的調用數量與CPU Cores滿載的數量完全相同。
* 由此可知，在硬體尚未滿載時，CPU Cores的數量與電腦效能成正比關係。而在設計系統程式時，也可在有效範圍內，將thread數量開到最大，使同步運算的程式達到最高效能。

:::

### 4.不同thread對執行時間的影響
* 如同上述提到，==程式調用的thread數量與CPU cores的數量是正比關係==，由此推論，在硬體有效範圍內，程式調用的thread數量越多，會將運算平均分散在CPU中，且CPU使用率會達到最高，程式執行速度理論上會越快。
* 實際測試後有以下發現，將此狀況分為兩部分
    * ==**當thread數量小於CPU Cores數量時**:== 
        * **現象**:++執行時間會隨著thread的數量增加而明顯下降。++
        * **推測原因**:在CPU可使用範圍內，每增加一個thread，就多了一個可同步執行程式的CPU Cores，可以分散每個thread須執行的程式內容，進而降低程式整體的執行時間。
    * ==**當thread數量大於CPU Cores數量時**:== 
        * **現象**:++執行時間與thread關聯性不高。++
        * **推測原因**:此時所有CPU Cores均已滿載，已沒有系統資源可被調用，故執行時間以優化到下限。

    
:::danger
**在測試過程有遇到兩個狀況會發生系統自行將執行中的程式killed:**
    1. 每個cores都滿載，且memory的使用率100%時
    2. cores使用率約在70~80%，且memory跟swap的使用率100%時
* 推測原因:
    * 因為是在虛擬機上執行，執行環境的記憶體最初只有4GB，而cores滿載時，memory也會隨著增加，並達到100%。
    * 在沒有使用虛擬機的情況下，系統會有swap的空間讓程式可以繼續運算，就算memory滿載，++只要swap可用空間還夠，就可以順利執行程式++。
    * 但在我架設的虛擬機執行環境中，swap空間只有1G，在memory使用率100%的情況下，swap使用率也很快會達到100%，造成++RAM空間不足，系統便會自動將執行中的程式kill，強制終止++。
* 解決方法：
將虛擬機的memory開到6GB，使程式執行中，memory使用率不會達到100%，便可順利執行。

:::
---
## 結論

:::info

|      | 當thread數量小於CPU Cores數量時 | 當thread數量大於CPU Cores數量時 |
| ---- | ------------------------------- | ------------------------------- |
| thread時間(平均)對總時間影響     |  ==呈正相關==                               |          影響不大                       |
| 寫檔時間對總時間影響 | 影響不大                            |==呈正相關==                            |
:::
* 由本次的作業觀察到，==CPU Cores數與程式調用的thread數量有很直接的關係==，將每個Cores使用到100%，可以提升程式的執行速度。但當cores已全數使用滿載時，再增加thread數量，則對程式的效能提升沒有太大幫助。
* **由上述可知，增加thread數量可以提升效能，但可增加的thread數量有上限，上限是CPU的核心數。**
* 在硬體設備方面，CPU的核心數越高，對於電腦的運算效率會有很大的影響。
* 在寫系統程式時，也可以善用此資訊，將可以同步執行的程式分散到thread中，以提升系統程式的效率。
* 另外，也發現一個小現象，一般的個人電腦CPU的一個核心會有兩個cores，但在虛擬機上面所看到的cores數量會剛好等於核心數，使得效能降低。



