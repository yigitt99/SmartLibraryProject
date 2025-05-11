#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <conio.h>
#include <chrono>
#include <thread>
#include "json.hpp"


using namespace std;
using namespace std::this_thread;
using namespace std::chrono;
using json = nlohmann::json;


// Kitap bilgilerini tutacak struct
struct Book {
    int kitap_no;
    string isbn;
    string baslik;
    string yazar;
};


// Kullanıcı bilgilerini temsil eden struct
struct Kullanici {
    int kullanici_no;
    string ad;
    string sifre;
    string rol;
};



// Fonksiyon prototipleri
vector<Book> loadBooksFromJson(const string& filename);
void saveBooksToJson(const string& filename, const vector<Book>& books);
vector<Kullanici> loadUsersFromJson(const string& filename);


void clear();
void intro();
bool isAdminLogin();
void kitapEkle();
void kitapSil();
void adminMenu();
bool login(bool& isAdmin);
void borrowBookJson();
void verifyIsbn();
void returnBook();
void normalMenu(bool isAdmin);


// JSON dosyasını okuyup kitapları vektöre atan fonksiyon
vector<Book> loadBooksFromJson(const string& filename) {
    vector<Book> books;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "books.json dosyasi acilamadi!" << endl;
        return books;
    }

    json j;
    file >> j;

    for (const auto& item : j) {
        books.push_back({
            item["kitap_no"],
            item["isbn"],
            item["baslik"],
            item["yazar"]
            });
    }

    return books;
}


// Kitapları JSON dosyasına kaydeden fonksiyon
void saveBooksToJson(const string& filename, const vector<Book>& books) {
    json j;
    for (const auto& book : books) {
        j.push_back({
            {"kitap_no", book.kitap_no},
            {"isbn", book.isbn},
            {"baslik", book.baslik},
            {"yazar", book.yazar}
            });
    }

    ofstream file(filename);
    if (file.is_open()) {
        file << setw(4) << j;
        file.close();
    }
    else {
        cerr << "Dosya acilamadi: " << filename << endl;
    }
}


// users.json dosyasından kullanıcıları okuyup vektöre aktaran fonksiyon
vector<Kullanici> loadUsersFromJson(const string& filename) {
    vector<Kullanici> kullanicilar;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "users.json dosyasi acilamadi!\n";
        return kullanicilar;
    }

    json j;
    file >> j;

    for (const auto& item : j) {
        kullanicilar.push_back({
            item["kullanici_no"],
            item["ad"],
            item["sifre"],
            item["rol"]
            });
    }

    return kullanicilar;
}


// Ekranı Temizleme Fonksiyonu
void clear() {
    system("cls");
}


// Giriş ekranı
void intro() {
    cout << "\n\t##      ## ####### ##      ######  ###### ####  #### #######";
    cout << "\n\t##      ## ##      ##      ##      ##  ## ##  ##  ## ##       ";
    cout << "\n\t##  ##  ## #####   ##      ##      ##  ## ##  ##  ## #####      ";
    cout << "\n\t##  ##  ## ##      ##      ##      ##  ## ##      ## ##           ";
    cout << "\n\t####  #### ####### ####### ######  ###### ##      ## #######     ";
    cout << "\n\n\t\t\t         !!!!!!!  !!!!!        ";
    cout << "\n\t\t\t           !!     !   !      ";
    cout << "\n\t\t\t           !!     !!!!!    ";

    cout << "\n\n\n\t\t********  LIBRARY   PROJECT  *******";
    cout << "\n\t\t====================================";
    cout << "\n\n\t\t\t  MADE BY  :-";
    cout << "\n\n\t\t\t  YIGIT EFE KILIC  ";
    cout << " \n\n\n\t\t         press any KEY to continue: Press any key to continue... ";
    cin.get();
    clear();
}


// Admin giriş kontrolü
bool isAdminLogin() {
    string adminUser = "admin";
    string adminPassword = "admin123";
    string username, password;

    cout << "\n\tAdmin Girisi:\n";
    cout << "\tKullanici adi: ";
    cin >> username;
    cout << "\tSifre: ";
    cin >> password;

    return (username == adminUser && password == adminPassword);
}


// Kitap ekleme fonksiyonu
void kitapEkle() {
    clear();
    cout << "\n\t\t\t\t\t-----------------------------------";
    cout << "\n\t\t\t\t\t|         K I T A P   E K L E      |";
    cout << "\n\t\t\t\t\t-----------------------------------";

    auto kitaplar = loadBooksFromJson("books.json");
    Book yeniKitap;

    // Kitap No kontrol döngüsü
    bool numaraKullaniliyor;
    do {
        numaraKullaniliyor = false;

        cout << "\n\n\tKitap No: ";
        while (!(cin >> yeniKitap.kitap_no)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n\tHatali giris! Lutfen sayi girin: ";
        }

        // Aynı numarayı kontrol et
        for (const auto& kitap : kitaplar) {
            if (kitap.kitap_no == yeniKitap.kitap_no) {
                cout << "\n\t\033[31mBu kitap no zaten kullaniliyor! Farkli bir no girin.\033[0m";
                numaraKullaniliyor = true;
                break;
            }
        }
    } while (numaraKullaniliyor);

    cin.ignore(); // Buffer temizleme

    cout << "\n\tISBN: ";
    getline(cin, yeniKitap.isbn);

    cout << "\n\tBaslik: ";
    getline(cin, yeniKitap.baslik);

    cout << "\n\tYazar: ";
    getline(cin, yeniKitap.yazar);

    kitaplar.push_back(yeniKitap);
    saveBooksToJson("books.json", kitaplar);

    cout << "\n\t\033[32mKitap basariyla eklendi!\033[0m";
}


// Kitap silme fonksiyonu
void kitapSil() {
    clear();
    cout << "\n\t\t\t\t\t-----------------------------------";
    cout << "\n\t\t\t\t\t|         K I T A P   S I L       |";
    cout << "\n\t\t\t\t\t-----------------------------------";

    vector<Book> kitaplar = loadBooksFromJson("books.json");

    if (kitaplar.empty()) {
        cout << "\n\t\033[33mKitap listesi bos!\033[0m";
        return;
    }

    cout << "\n\tMevcut Kitaplar:\n";
    for (const auto& kitap : kitaplar) {
        cout << "\n\t" << kitap.kitap_no << " | " << kitap.baslik
            << " - " << kitap.yazar << " (ISBN: " << kitap.isbn << ")";
    }

    int silinecekNo;
    cout << "\n\n\tSilinecek kitap no (0: İptal): ";

    while (!(cin >> silinecekNo)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n\t\033[31mHatali giris! Lutfen sayi girin:\033[0m ";
    }

    if (silinecekNo == 0) {
        cout << "\n\t\033[33mSilme islemi iptal edildi.\033[0m";
        return;
    }

    bool bulundu = false;
    kitaplar.erase(remove_if(kitaplar.begin(), kitaplar.end(),
        [&silinecekNo, &bulundu](const Book& k) {
            if (k.kitap_no == silinecekNo) {
                bulundu = true;
                return true;
            }
            return false;
        }),
        kitaplar.end());

    if (bulundu) {
        saveBooksToJson("books.json", kitaplar);
        cout << "\n\t\033[32mKitap basariyla silindi!\033[0m";

        vector<Book> oduncKitaplar = loadBooksFromJson("odunc.json");
        oduncKitaplar.erase(remove_if(oduncKitaplar.begin(), oduncKitaplar.end(),
            [silinecekNo](const Book& k) { return k.kitap_no == silinecekNo; }),
            oduncKitaplar.end());
        saveBooksToJson("odunc.json", oduncKitaplar);
    }
    else {
        cout << "\n\t\033[31mBu numarada kitap bulunamadi!\033[0m";
    }
}


// Admin menüsü
void adminMenu() {
    int choice;
    do {
        clear();
        cout << "\n\t\t\t\t\t-----------------------------------";
        cout << "\n\t\t\t\t\t|       A D M I N   M E N U       |";
        cout << "\n\t\t\t\t\t-----------------------------------";
        cout << "\n\n\t1. Kitap Ekle";
        cout << "\n\t2. Kitap Sil";
        cout << "\n\t3. Odunc Listele";
        cout << "\n\t4. Ana Menuye Don";
        cout << "\n\n\tLutfen 1 ile 4 arasinda bir secim yapiniz: ";
        cin >> choice;

        switch (choice) {
        case 1:
            kitapEkle();
            break;
        case 2:
            kitapSil();
            break;
        case 3:
            // Odunc listesi gösterme fonksiyonu burada çağır
            break;
        case 4:
            return;
        default:
            cout << "\n\tGecersiz secim, tekrar deneyin.\n";
            break;
        }

        cout << "\n\tDevam etmek icin bir tusa basin...";
    } while (true);
}


// Kullanıcı giriş fonksiyonu
bool login(bool& isAdmin) {
    vector<Kullanici> kullanicilar = loadUsersFromJson("users.json");

    if (kullanicilar.empty()) {
        cout << "\n\tKullanici listesi bos veya dosya bulunamadi.\n";
        return false;
    }

    string girilenAd, girilenSifre;

    do {
        clear();
        cout << "\n\t\t\t\t\t---------------------------------------------";
        cout << "\n\t\t\t\t\t|                                           |";
        cout << "\n\t\t\t\t\t|               W E L C O M E               |";
        cout << "\n\t\t\t\t\t|                                           |";
        cout << "\n\t\t\t\t\t---------------------------------------------";

        cout << "\n\n\n\t\t\t\t\t\tKullanici adinizi girin: ";
        cin >> girilenAd;

        cout << "\n\n\t\t\t\t\t\tSifrenizi girin: ";
        cin >> girilenSifre;

        for (const auto& k : kullanicilar) {
            if (k.ad == girilenAd && k.sifre == girilenSifre) {
                clear();
                cout << "\n\t\t\t\t\t---------------------------------------------";
                cout << "\n\t\t\t\t\t|               P L E A S E                 |";
                cout << "\n\t\t\t\t\t|                                           |";
                cout << "\n\t\t\t\t\t|                   W A I T . . .           |";
                cout << "\n\t\t\t\t\t---------------------------------------------";

                cout << "\033[32m\n\t\t\t\t\t    Giris basarili. Hosgeldiniz, " << girilenAd << "!\033[0m" << endl;
                cout << "\n\t\t\t\t\tLutfen Bekleyin Menuye Aktariliyorsunuz...";
                sleep_for(seconds(1));

                // Admin kontrolü
                isAdmin = (k.rol == "admin");
                return true; // Başarılı giriş
            }
        }

        // Hatalı giriş
        clear();
        cout << "\n\t\t\t\t\t\033[31m    Hatali Giris! Lutfen tekrar deneyin.\033[0m" << endl;
        sleep_for(seconds(1));

    } while (true); // Doğru giriş yapılana kadar döngü devam eder
}


// Kitap ödünç alma fonksiyonu
void borrowBookJson() {
    vector<Book> books = loadBooksFromJson("books.json");

    if (books.empty()) {
        cout << "\n\tKitap listesi bos veya dosya bulunamadi.\n";
        return;
    }

    while (true) {
        clear();
        cout << "\n\t\t\t\t\t---------------------------------------------";
        cout << "\n\t\t\t\t\t|                M E V C U T                |";
        cout << "\n\t\t\t\t\t|                                           |";
        cout << "\n\t\t\t\t\t|              K I T A P L A R              |";
        cout << "\n\t\t\t\t\t---------------------------------------------";

        for (const auto& book : books) {
            cout << "\n\n\t\t" << book.kitap_no << ". " << book.baslik << " - " << book.yazar;
        }
        cout << "\n\n\t\t0. Ana Menuye Don\n";

        int secilenId;
        cout << "\n\n\t\tLutfen almak istediginiz kitap ID'sini girin (0: Menuye don): ";
        cin >> secilenId;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n\tGecersiz giris! Lutfen sayi giriniz.\n";
            continue;
        }

        if (secilenId == 0) {
            cout << "\n\tAna menuye donuluyor...\n";
            break;
        }

        auto it = find_if(books.begin(), books.end(), [secilenId](const Book& b) {
            return b.kitap_no == secilenId;
            });

        if (it == books.end()) {
            cout << "\n\tBu ID'ye sahip bir kitap bulunamadi. Lutfen tekrar deneyin.\n";
            continue;
        }

        auto now = system_clock::now();
        time_t now_time = system_clock::to_time_t(now);
        tm borrow_date;
        localtime_s(&borrow_date, &now_time);

        time_t due_time = now_time + 15 * 24 * 60 * 60;
        tm due_date;
        localtime_s(&due_date, &due_time);

        stringstream teslimAlma, teslimTarihi;
        teslimAlma << put_time(&borrow_date, "%d/%m/%Y");
        teslimTarihi << put_time(&due_date, "%d/%m/%Y");

        json yeniKayit = {
            {"kitap_no", it->kitap_no},
            {"isbn", it->isbn},
            {"baslik", it->baslik},
            {"yazar", it->yazar},
            {"teslim_alma", teslimAlma.str()},
            {"teslim_tarihi", teslimTarihi.str()}
        };

        json oduncListesi;
        ifstream inFile("odunc.json");
        if (inFile.is_open()) {
            inFile >> oduncListesi;
            inFile.close();
        }
        else {
            oduncListesi = json::array();
        }

        oduncListesi.push_back(yeniKayit);
        ofstream outFile("odunc.json");
        outFile << setw(4) << oduncListesi << endl;
        outFile.close();

        cout << "\033[32m\n\n\t\tKitap Alindi:\n\033[0m";
        cout << "\n\t\tBaslik: " << it->baslik;
        cout << "\n\t\tISBN: " << it->isbn;
        cout << "\n\t\tTeslim Alinan Tarih: " << teslimAlma.str();
        cout << "\n\t\tTeslim Edilmesi Gereken Tarih: " << teslimTarihi.str();

        cout << "\n\n\tDevam etmek icin bir tusa basin...";
        cin.ignore();
        cin.get();
        break;
    }
}


// ISBN doğrulama fonksiyonu
void verifyIsbn() {
    vector<Book> books = loadBooksFromJson("books.json");

    if (books.empty()) {
        cout << "\n\tKitap listesi bos veya dosya bulunamadi.\n";
        return;
    }

    string isbn;
    do {
        clear();
        cout << "\n\t\t\t\t\t---------------------------------------------";
        cout << "\n\t\t\t\t\t|                I S B N                    |";
        cout << "\n\t\t\t\t\t|                                           |";
        cout << "\n\t\t\t\t\t|              D O G R U L A M A            |";
        cout << "\n\t\t\t\t\t---------------------------------------------";

        cout << "\n\tLutfen dogrulamak istediginiz ISBN numarasini girin (0: Ana menuye don): ";
        cin >> isbn;

        if (isbn == "0") {
            cout << "\n\tAna menuye donuluyor...\n";
            break;
        }

        auto it = find_if(books.begin(), books.end(), [isbn](const Book& b) {
            return b.isbn == isbn;
            });

        if (it != books.end()) {
            cout << "\033[32m\n\tISBN dogrulandi! Kitap bilgileri:\033[0m\n";
            cout << "\tBaslik: " << it->baslik << "\n";
            cout << "\tYazar: " << it->yazar << "\n";
            break;
        }
        else {
            cout << "\033[31m\n\tISBN bulunamadi. Lutfen tekrar deneyin.\033[0m\n";
            sleep_for(seconds(1));
        }
    } while (isbn != "0");

    cout << "\n\tDevam etmek icin bir tusa basin...";
    cin.ignore();
    cin.get();
}


// Kitap iade fonksiyonu
void returnBook() {
    clear();
    vector<Book> borrowedBooks;
    json oduncListesi;

    ifstream inFile("odunc.json");
    if (inFile.peek() == ifstream::traits_type::eof()) {
        cout << "\n\tHenuz hic kitap odunc alinmamis.\n";
        cout << "\n\tDevam etmek icin bir tusa basin...";
        cin.ignore();
        cin.get();
        return;
    }
    else {
        inFile >> oduncListesi;
        inFile.close();
    }

    cout << "\n\t\tOdunc Alinan Kitaplar:\n";
    for (const auto& kitap : oduncListesi) {
        cout << "\n\tKitap No: " << kitap["kitap_no"]
            << " | Baslik: " << kitap["baslik"]
            << " | ISBN: " << kitap["isbn"]
            << " | Teslim Tarihi: " << kitap["teslim_tarihi"];
    }

    cout << "\n\n\t0. Ana Menuye Don";
    cout << "\n\n\tIade etmek istediginiz kitap ID'sini giriniz: ";

    string girdi;
    cin.ignore();
    getline(cin, girdi);

    if (girdi == "0") return;

    bool bulundu = false;
    for (auto it = oduncListesi.begin(); it != oduncListesi.end(); ++it) {
        string idStr = to_string((*it)["kitap_no"].get<int>());
        string isbnStr = (*it)["isbn"].get<string>();

        if (girdi == idStr || girdi == isbnStr) {
            oduncListesi.erase(it);
            bulundu = true;
            break;
        }
    }

    if (bulundu) {
        ofstream outFile("odunc.json");
        outFile << setw(4) << oduncListesi;
        outFile.close();
        cout << "\n\t\033[32mKitap basariyla iade edildi!\033[0m\n";
    }
    else {
        cout << "\n\t\033[31mGirdiginiz ID veya ISBN ile eslesen bir kitap bulunamadi.\033[0m\n";
    }

    cout << "\n\tDevam etmek icin bir tusa basin...";
    cin.get();
}


// Normal kullanıcı menüsü
void normalMenu(bool isAdmin) {
    int choice;

    do {
        clear();
        cout << "\n\n\n\tANA MENU";
        cout << "\n\n\t1. Kitap Odunc Al";
        cout << "\n\n\t2. ISBN DOGRULAMA";
        cout << "\n\n\t3. IADE ISLEMLERI";
        if (isAdmin) {
            cout << "\n\n\t4. Admin Menusune Git";
        }
        cout << "\n\n\t(5) CIKIS";
        cout << "\n\n\tLutfen 1 ile 4 arasinda bir sayi giriniz: ";
        cin >> choice;

        if (choice < 1 || choice > 5) {
            cout << "\n\n\tGecersiz secim! Lutfen tekrar deneyin.\n";
            cin.ignore();
            cin.get();
            continue;
        }

        switch (choice) {
        case 1:
            borrowBookJson();
            break;
        case 2:
            verifyIsbn();
            break;
        case 3:
            returnBook();
            break;
        case 4:
            if (isAdmin) {
                adminMenu();
            }
            else {
                cout << "\n\tCikis yapiliyor...\n";
            }
            break;
        case 5:
            cout << "\t\t\tCikis Yapiliyor..." << endl;
            sleep_for(seconds(2));
            break;
        }
    } while (choice != 5);
}



// Main fonksiyonu
int main() {

    intro();

    bool isAdmin = false;


    login(isAdmin);

    normalMenu(isAdmin);

    return 0;
}
