# *How to Run the program*
Berikut adalah cara untuk melakukan segala *setup*, mulai dari SSH, cara menggunakan server, hingga apa yang harus dilakukan jika melakukan *additional changes*.

## *Initialize* SSH
1. *Download* 2 file ssh  [disini](https://drive.google.com/drive/folders/1rCcgEeF72cAzuxSro-eWT64wC0jnG7h6?usp=sharing), kemudian *download* yang ```k02-05``` dan ```k02-05.pub```.
2. Jika menggunakan WSL, setelah di *download*, pindahkan SSH key (keduanya) ke *directory* WSL ```(/home/[username]/)```. Bukan pada *directory* Windows ```(/mnt/[drive letter]/[any]/**/*)```. Masukkan kedanya di folder ```.ssh```.
3. *Edit permission* SSH key dengan ```chmod```.
    ```bash
    $ chmod 600 ~/.ssh/k02-05
    $ chmod 644 ~/.ssh/k02-05.pub
    ```
4. *Add* SSH key nya agar tercatat sebagai *known host*.
    ```bash
    $ ssh-add ~/.ssh/k02-05
    ```
5. Bikin *authorization keys* SSH nya *available*.
    ```bash
    $ cd ~/.ssh/
    $ sudo nano authorized_keys
    ```
    Selanjutnya akan muncul *code editor* nano. Jika berhasil, sekarang buka ```k02-05.pub```, lalu salin semua isinya dan *paste* di *code editor* nano.
    Jika sudah, jangan lupa untuk melakukan *save* (```Ctrl + S```) dan *exit* (```Ctrl + X```).
6. Sekarang setup `sshd_config` nya agar setiap coba akses ke server tidak selalu meminta *password*.
    ```bash
    $ cd ..
    $ sudo nano /etc/ssh/sshd_config
    ```
    Selanjutnya akan muncul *code editor* nano. Jika berhasil, masukkan 2 *permission* ini ke *code editor* nano nya
    ```bash
    PasswordAuthentication no
    PubkeyAuthentication yes
    ```
    Jika sudah, jangan lupa untuk melakukan *save* (```Ctrl + S```) dan *exit* (```Ctrl + X```).
7. Jika semua langkah diatas sudah dilakukan, SSH sudah siap untuk menembak server!

## Menggunakan server
1. Hubungkan dengan salah satu server menggunakan SSH melewati IP publik. 
IP publik yang disediakan asisten, ada 2 yaitu.
    ```
    4.145.183.206
    4.145.178.6
    ```
    Kita akan menggunakan IP publik yang pertama, lakukan dengan *run command* berikut.
    ```base
    $ ssh k02-05@4.145.183.206
    ```
    Jika sudah berhasil, seharusnya program akan minta *permission*, ketik ```yes```.
2. Dalam kondisi masih terhubung dengan salah satu server, lakukan SSH ke semua server dengan IP *private*.
    - Jika SSH key *public* 4.145.183.206, maka *private* nya ada 10.0.1.7 dan 10.0.1.4
    - Jika SSH key public 4.145.178.6, maka *private* nya ada 10.0.1.6 dan 10.0.1.5 <br>
    
    Caranya, tanpa merubah apapun, ketik dua *command* berikut secara bergantian (dalam kasus ini, digunakan IP publik pertama)
    ```base
    $ ssh k02-05@10.0.1.7
    $ ssh k02-05@10.0.1.4
    ```
3. Putuskan koneksi SSH ke server dan kembali ke local. Caranya tinggal ketik ```exit``` hingga kembali.
    ```base
    $ exit
    ```
4. Masuk ke *directory* tucil (dalam kondisi menggunakan WSL), Buat file hostname dengan isi semua IP privat yang dipisahkan dengan *newline*. Dapat dilakukan dengan menjalankan *command* berikut.
    ```bash
    $ echo -e "10.0.1.7\n10.0.1.4" > hostname
    ```
5. *Compile* program yang baru saja dibuat dengan *command* berikut.
    - Untuk Serial, jalankan *command* dibawah
        ```bash
        $ make compile-serial
        ```
    - Untuk MPI, jalankan *command* dibawah
        ```bash
        $ make compile-mpi
        ```
    Selanjutnya, salin *executable*, *test cases*, makefile, dan *result* *folder* ke semua server menggunakan `scp`. Dapat dilakukan dengan menjalankan command berikut.
    ```bash
    $ make copy-server
    ```
6. Jika langkah sebelumnya berhasil, maka Anda akan masuk ke dalam server publik. Langkah berikutnya, adalah mendistribusikan isinya ke semua server private dengan mengetikkan *command* berikut.
    ```bash
    $ make copy-branch-server
    ```
    Kemudian untuk menjalankan program ketikkan *command* dibawah.
    - Untuk Serial, jalankan *command* dibawah
        ```bash
        $ make serial
        ```
    - Untuk MPI, chmod +x executable dan jalankan dengan mpirun, pada *command* dibawah, MPI dijalankan dengan 2 process
        ```bash
        $ make mpi
        ```
7. Jika semua prosedur diatas dijalankan, maka seharusnya program dapat berjalan dengan baik!

## Additional changes
Jika sebelumnya sudah melakukan prosedur diatas dan melakukan perubahan yang ingin di *update*, maka kedepannya yang perlu dilakukan hanya *run command* berikut.
1. Untuk Serial, jalankan *command* di bawah
    ```bash
    $ make compile-serial
    $ make copy-server
    $ make copy-branch-server
    $ make serial
    ```
2. Untuk MPI, jalankan *command* di bawah
    ```bash
    $ make compile-mpi
    $ make copy-server
    $ make copy-branch-server
    $ make mpi
    ```
    Untuk menjalankannya secara lokal, jalankan *command* dibawah
    ```bash
    $ make compile-mpi
    $ make mpi-local
    ```
    Untuk menjalankan debugger dengan valgrind (pastikan sudah menginstall sebelumnya), jalankan *command* dibawah
    ```bash
    $ make compile-mpi
    $ make mpi-debug
    ```
    Anda juga dapat mengubah konfigurasi jumlah proses dengan mengubah langsung konfigurasi makefile.