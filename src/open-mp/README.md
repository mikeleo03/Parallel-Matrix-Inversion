# 🔢📈 Tugas Kecil 2 - Paralel Invers Matrix dengan OpenMP
> Tugas Kecil 2 IF3230 Sistem Paralel dan Terdistribusi, Melakukan Paralelisasi Invers Matrix Metode Gauss-Jordan dengan OpenMP.

## ❓ &nbsp;Apa itu OpenMP?
OpenMP (Open Multi-Processing) adalah sebuah antarmuka pemrograman aplikasi (API) yang mendukung pemrograman multiproses pada sistem *shared-memmory*. OpenMP dirancang untuk membuat pengembangan aplikasi paralel lebih mudah dan lebih efisien dengan menggunakan direktif *preprocessor* dan variabel lingkungan yang bekerja bersama untuk membangun blok bangunan dasar paralelisme dalam kode. OpenMP memungkinkan pengembang untuk menulis kode yang dapat dijalankan secara paralel pada berbagai inti prosesor secara bersamaan. Dengan demikian, aplikasi dapat memanfaatkan potensi penuh dari arsitektur multiprosesor, meningkatkan kinerja aplikasi secara signifikan. Sebagai sebuah kerangka kerja yang luas digunakan dalam berbagai aplikasi komputasi, OpenMP telah terbukti efektif dalam menyederhanakan proses pengembangan aplikasi paralel, yang pada gilirannya meningkatkan produktivitas dan efisiensi pengembangan perangkat lunak.

## 🤔 &nbsp;Bagaimana Implementasinya dalam Invers Matrix?
TODO

## 💡 &nbsp;Detail Prosedur Implementasi
TODO

## 🔍 &nbsp;Analisis Implementasi Solusi
TODO

## 📸 &nbsp;Dokumentasi Hasil Eksekusi
Hasil ekesekusi lokal  (*data atas menggunakan serial dan data bawah menggunakan OpenMP*).

![demo1](../../public/openmp-local.png)

Hasil ekekusi menggunakan server.

![demo2](../../public/openmp-server.png)

Secara umum diperoleh *speed-up* hingga $4 \times$ lipat! 🤩