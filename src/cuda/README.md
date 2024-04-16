# 🔢📈 Tugas Kecil 2 - Paralel Invers Matrix dengan CUDA
> Tugas Kecil 2 IF3230 Sistem Paralel dan Terdistribusi, Melakukan Paralelisasi Invers Matrix Metode Gauss-Jordan dengan Cuda.

## ❓ &nbsp;Apa itu CUDA?
CUDA (Compute Unified Device Architecture) adalah sebuah platform dan API yang dikembangkan oleh NVIDIA untuk memungkinkan penggunaan unit pemrosesan grafis (GPU) untuk pemrosesan selain grafis. CUDA memberikan pengembang kemampuan untuk memanfaatkan GPU untuk komputasi berkinerja tinggi (HPC) dan komputasi paralel di aplikasi yang berbeda-beda, seperti simulasi, analisis data, dan pembelajaran mesin.Secara khusus, CUDA menyediakan perangkat lunak dengan cara langsung untuk mengirimkan perintah ke GPU, tidak hanya untuk rendering grafis, tetapi juga untuk melakukan perhitungan kompleks secara paralel. Ini memanfaatkan ribuan core pada GPU modern untuk melakukan operasi matematika yang sangat efisien dan lebih cepat dibandingkan dengan CPU biasa untuk tugas-tugas tertentu. CUDA juga mendukung berbagai bahasa pemrograman, termasuk C, C++, dan Fortran, serta berbagai pustaka yang memudahkan pengembangan perangkat lunak yang berorientasi pada performa tinggi.

## 🤔 &nbsp;Bagaimana Implementasinya dalam Invers Matrix?
Prosedur inversi matriks menggunakan CUDA memanfaatkan kemampuan pemrosesan paralel GPU untuk meningkatkan efisiensi secara signifikan. Implementasi ini terutama menggunakan teknik eliminasi Gauss untuk mengubah matriks menjadi bentuk eselon tereduksi, yang kemudian diikuti oleh normalisasi untuk membentuk matriks identitas. Di samping matriks identitas tersebut terletak invers dari matriks asli. Seluruh proses ini dapat dilakukan dengan cepat berkat alokasi dan distribusi thread yang efisien. Setiap thread bertanggung jawab atas perhitungan yang diperlukan pada baris tertentu. Tahapan awal melibatkan penggunaan Kernel CUDA untuk eliminasi Gauss, yang mengurangi elemen di bawah pivot menjadi nol dengan mengurangi kelipatan baris pivot dari baris lain. Ini diikuti oleh reduce_to_unit_kernel di kernel kedua, yang mengubah matriks di sebelah kiri menjadi matriks identitas dan matriks di sebelah kanan menjadi matriks invers. Proses ini diakhiri dengan sinkronisasi dan pemindahan hasil dari GPU ke CPU.

## 💡 &nbsp;Detail Prosedur Implementasi
1. Terima masukan sebuah matriks berukuran $n \times n$ dan inisiasi pula matriks identitas $n \times n$ di sisi kanannya. Hasil akhir yang terbentuk adalah sebuah matriks berukuran $n \times 2n$.
2. 
   - 
   - 
   - 
   - 
   - 
3. 
   - 
   - 

## 🔍 &nbsp;Analisis Implementasi Solusi
Dalam implementasi algoritma inversi matriks menggunakan CUDA, efisiensi diperoleh melalui penggunaan pemrosesan paralel di GPU. Penggunaan memori GPU dilakukan secara efektif, dengan alokasi matriks pada memori global, yang memungkinkan akses cepat dan paralel oleh semua thread yang berjalan. Pengurangan elemen matriks menjadi unit melalui kernel reduce_to_unit_kernel memastikan bahwa setiap elemen pada diagonal utama dan elemen lain pada baris pivot dinormalisasi, yang dilakukan oleh thread yang berbeda secara paralel untuk meningkatkan efisiensi proses. Sinkronisasi antar thread dilakukan setelah setiap langkah eliminasi dan normalisasi untuk memastikan bahwa semua operasi pada matriks selesai sebelum melanjutkan ke pivot berikutnya. Hal ini mengurangi kesalahan yang mungkin terjadi akibat operasi yang belum selesai sebelum memulai operasi lain. Proses paralel ini sangat mengurangi waktu yang diperlukan untuk melakukan inversi matriks, terutama untuk matriks besar, karena memanfaatkan ribuan thread yang dapat beroperasi secara simultan. Selain itu, transfer data antara CPU dan GPU dioptimalkan untuk mengurangi latensi dan memastikan bahwa operasi dapat berjalan tanpa hambatan signifikan.

## 📸 &nbsp;Dokumentasi Hasil Eksekusi
Hasil ekesekusi lokal  (*data atas menggunakan serial dan data bawah menggunakan CUDA*).

![demo1](../../public/cuda-local.png)

Hasil ekekusi menggunakan server.

![demo2](../../public/openmp-server.png)

Secara umum diperoleh *speed-up* hingga $15 \times$ lipat! 🤩
