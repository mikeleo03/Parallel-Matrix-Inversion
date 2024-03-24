# Tugas Kecil - Paralel Inverse Matrix

Template Tugas Kecil IF3230 Sistem Paralel dan Terdistribusi

## Cara menjalankan

Bisa dilihat di README-run.md

## Algoritma

### OpenMPI

1. Terima input matriks (sebesar n x n) dan tambahkan matriks identitas (sebesar n x n juga) di sisi kanannya.
2. Bagi baris-baris matriks ke seluruh proses yang ada secara merata.
3. Untuk tiap baris i pada matriks :
    - Tiap proses mencari baris pivot terbaik dari semua baris yang dimilikinya.
    - Kirimkan pivot terbaik tersebut ke proses root (dalam kasus ini proses 0).
    - Proses 0 mencari pivot terbaik dari keseluruhan calon pivot yang diterima.
    - Bagi baris pivot dengan *leading coefficient*-nya.
    - Kirimkan baris pivot yang sudah dibagi ke seluruh proses.
    - Tukar baris pivot dengan baris i jika tidak sama.
    - Tiap proses mengurangi baris yang dimilikinya dengan hasil perkalian baris pivot sehingga semua nilai non-diagonal pada kolom i menjadi 0.
4. Setelah seluruh baris selesai diproses, masing-masing proses mengirimkan baris yang dimiliki ke proses 0.
