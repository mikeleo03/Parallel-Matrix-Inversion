# How to Run the program
Tolong dibaca dulu supaya ga bingung harus ngapain.

## Initialize SSH
1. Download 2 file ssh di drivenya, bisa [disini](https://drive.google.com/drive/folders/1rCcgEeF72cAzuxSro-eWT64wC0jnG7h6?usp=sharing), terus download yang ```k02-05``` dan ```k02-05.pub```.
2. Kalo pake WSL, setelah di download, pindah SSH key (keduanya) ke directory WSL ```(/home/[username]/)```. Bukan pada directory Windows ```(/mnt/[drive letter]/[any]/**/*)```. Masukin di folder ```.ssh```
3. Edit permission SSH key dengan ```chmod```
    ```bash
    $ chmod 600 ~/.ssh/k02-05
    $ chmod 644 ~/.ssh/k02-05.pub
    ```
4. Add SSH key nya biar kecatat sabagi known host
    ```bash
    $ ssh-add ~/.ssh/k02-05
    ```
5. Bikin authorization keys SSH nya available
    ```bash
    $ cd ~/.ssh/
    $ sudo nano authorized_keys
    ```
    Harusnya kebuka code editor nano. Kalo berhasil, sekarang buka ```k02-05.pub```, terus copy semua isinya dan paste di code editor nano nya.
    Kalo udah jangan lupa buat di save (```Ctrl + S```) dan exit (```Ctrl + X```).
6. Sekarang setup sshd_conifg nya biar setiap coba akses ke server dia ga selalu minta password
    ```bash
    $ cd ..
    $ sudo nano /etc/ssh/sshd_config
    ```
    Harusnya kebuka code editor nano. Kalo berhasil, masukin 2 permission ini ke code editor nano nya
    ```bash
    PasswordAuthentication no
    PubkeyAuthentication yes
    ```
    Kalo udah jangan lupa buat di save dan exit.
7. Kalo semua diatas udah dilakukan, SSH mu sudah siap untuk nembak server!

## Menggunakan server
1. Hubungkan dengan salah satu server menggunakan SSH melewati IP publik. 
IP publik yang disediakan asisten ada 2 yaitu.
    ```
    4.145.183.206
    4.145.178.6
    ```
    Aku bakal kasih contoh pake IP publik yang pertama, lakukan dengan run command berikut.
    ```base
    $ ssh k02-05@4.145.183.206
    ```
    Kalo udah masuk, harusnya dia minta permission, ketik ```yes``` aja trus nanti masuk.
2. Dalam kondisi masih terhubung dengan salah satu server, lakukan SSH ke semua server dengan IP private.
    - Kalo SSH key public 4.145.183.206, maka privatenya ada 10.0.1.7 dan 10.0.1.4
    - Kalo SSH key public 4.145.178.6, maka privatenya ada 10.0.1.6 dan 10.0.1.5 <br>
    
    Caranya, tanpa merubah apapun, ketik dua command berikut secara bergantian (dalam case ini aku pake IP publik atas)
    ```base
    $ ssh k02-05@10.0.1.7
    $ ssh k02-05@10.0.1.4
    ```
3. Putuskan koneksi SSH ke server dan kembali ke local. Caranya tinggal ketik ```exit``` 3 kali.
    ```base
    $ exit
    ```
4. Masuk ke directory tucil (dalam kondisi pake WSL), Buat file hostname dengan isi semua IP privat yang dipisahkan dengan newline. Dapat dilakukan dengan menjalankan command berikut.
    ```bash
    $ echo -e "10.0.1.7\n10.0.1.4" > hostname
    ```
5. Compile program dan salin executable serta test case ke semua server menggunakan scp. Dapat dilakukan dengan menjalankan command berikut.
    ```bash
    $ scp -r bin k02-05@4.145.183.206:/home/k02-05
    $ scp hostname k02-05@4.145.183.206:/home/k02-05
    $ scp -r test_cases k02-05@4.145.183.206:/home/k02-05
    $ scp makefile k02-05@4.145.183.206:/home/k02-05
    ```
6. Lakukan SSH lagi ke server utama
    ```bash
    $ ssh k02-05@4.145.183.206
    ```
    - Untuk Serial, jalankan command dibawah
        ```bash
        $ make serial
        ```
    - Untuk MPI, chmod +x executable dan jalankan dengan mpirun, pada command dibawah, MPI dijalankan dengan 2 process
        ```bash
        $ mpi run
        ```
7. Jika semua prosedur diatas dijalankan, maka seharusnya program dapat berjalan dengan baik!

## Additional changes
Jika sebelumnya sudah melakukan prosedur diatas dan melakukan perubahan yang ingin diupdate, maka kedepannya yang perlu dilakukan hanya run command berikut.
1. Untuk Serial, jalankan command di bawah
    ```bash
    $ make compile-serial
    $ make copy-server
    $ make serial
    ```
2. Untuk MPI, jalankan command di bawah
    ```bash
    $ make compile-mpi
    $ make copy-server
    $ mpi run
    ```