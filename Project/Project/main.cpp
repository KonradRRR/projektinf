#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Sta³e do ustawienia wielkoœci okna i mapy
const int ROZMIAR_KAFELKA = 50;  // wielkoœæ jednego kafelka w pikselach
const int SZEROKOSC_MAPY = 21;   // ile kafelków w poziomie
const int WYSOKOSC_MAPY = 15;    // ile kafelków w pionie
const int SZEROKOSC_OKNA = SZEROKOSC_MAPY * ROZMIAR_KAFELKA;
const int WYSOKOSC_OKNA = WYSOKOSC_MAPY * ROZMIAR_KAFELKA;
const float CZAS_GONIENIA = 5.0f;     // jak d³ugo duszki goni¹ gracza
const float PRZERWA_GONIENIA = 15.0f; // co ile czasu duszki zaczynaj¹ goniæ

// Mapa gry: 1 = œciana, 0 = punkt
int mapa[WYSOKOSC_MAPY][SZEROKOSC_MAPY] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,2,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,2,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,1,1,0,0,0,0,0,1,1,1,0,1,1,1,1},
    {1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1},
    {1,0,2,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,2,0,1},
    {1,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Struktura reprezentuj¹ca punkt do zebrania
// Ka¿dy punkt ma swoj¹ pozycjê i mo¿e byæ aktywny lub nie
struct Punkt {
    sf::Vector2i pozycja;  // pozycja na mapie
    sf::CircleShape ksztalt;  // wygl¹d punktu
    bool aktywny;  // czy punkt zosta³ ju¿ zebrany

    // Konstruktor - tworzy nowy punkt w podanej pozycji
    Punkt(sf::Vector2i poz) : pozycja(poz), aktywny(true) {
        ksztalt = sf::CircleShape(5);  // kó³ko o promieniu 5 pikseli
        ksztalt.setFillColor(sf::Color::White);
        // ustawiam pozycjê punktu na œrodku kafelka
        ksztalt.setPosition(
            poz.x * ROZMIAR_KAFELKA + ROZMIAR_KAFELKA / 2 - 5,
            poz.y * ROZMIAR_KAFELKA + ROZMIAR_KAFELKA / 2 - 5
        );
    }
};

// Funkcja zwraca losowy kierunek ruchu (góra, dó³, lewo, prawo)
sf::Vector2i losowy_kierunek() {
    int kier = rand() % 4;  // losujê liczbê od 0 do 3
    if (kier == 0) return { 0, -1 };  // góra
    if (kier == 1) return { 0, 1 };   // dó³
    if (kier == 2) return { -1, 0 };  // lewo
    return { 1, 0 };  // prawo
}

// Funkcja liczy odleg³oœæ miêdzy dwoma punktami
// U¿ywam wzoru na odleg³oœæ euklidesow¹
float odleglosc(sf::Vector2i a, sf::Vector2i b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Funkcja znajduje najlepszy kierunek do celu
// Sprawdza wszystkie mo¿liwe ruchy i wybiera ten, który najbardziej zbli¿a do celu
sf::Vector2i znajdz_kierunek_do_celu(sf::Vector2i obecna, sf::Vector2i cel) {
    // Mo¿liwe kierunki ruchu
    std::vector<sf::Vector2i> mozliwe_kierunki = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };

    float min_odl = 99999.0f;  // du¿a liczba na pocz¹tek
    sf::Vector2i najlepszy_kier = losowy_kierunek();  // jakby nic nie znalaz³

    // Sprawdzam ka¿dy mo¿liwy kierunek
    for (const auto& kier : mozliwe_kierunki) {
        sf::Vector2i nowa_poz = obecna + kier;
        // Sprawdzam czy nowa pozycja jest ok (nie wychodzi poza mapê i nie jest œcian¹)
        if (nowa_poz.x >= 0 && nowa_poz.x < SZEROKOSC_MAPY &&
            nowa_poz.y >= 0 && nowa_poz.y < WYSOKOSC_MAPY &&
            mapa[nowa_poz.y][nowa_poz.x] != 1) {
            float odl = odleglosc(nowa_poz, cel);
            if (odl < min_odl) {
                min_odl = odl;
                najlepszy_kier = kier;
            }
        }
    }

    return najlepszy_kier;
}

// Struktura reprezentuj¹ca ducha
struct Duch {
    sf::CircleShape ksztalt;      // jak duch wygl¹da
    sf::Vector2i pozycja;         // gdzie jest
    sf::Vector2i kierunek;        // dok¹d idzie
    bool goni;                    // czy goni gracza
    sf::Color normalny_kolor;     // kolor ducha jak nie goni

    // Konstruktor - ustawia pocz¹tkowe wartoœci ducha
    Duch(sf::Color kolor, sf::Vector2i start_poz, sf::Vector2i start_kier)
        : normalny_kolor(kolor), goni(false) {
        ksztalt = sf::CircleShape(ROZMIAR_KAFELKA / 2 - 2);
        ksztalt.setFillColor(kolor);
        pozycja = start_poz;
        kierunek = start_kier;
    }

    // Funkcja aktualizuje kierunek ruchu ducha
    void zmien_kierunek(const sf::Vector2i& pozycja_gracza, bool tryb_gonienia) {
        if (tryb_gonienia) {
            // W trybie gonienia duch próbuje z³apaæ gracza
            kierunek = znajdz_kierunek_do_celu(pozycja, pozycja_gracza);
            ksztalt.setFillColor(sf::Color::Red);  // duch robi siê czerwony
        }
        else {
            // Normalny ruch - losowy gdy napotka œcianê
            if (mapa[pozycja.y + kierunek.y][pozycja.x + kierunek.x] == 1) {
                kierunek = losowy_kierunek();
            }
            ksztalt.setFillColor(normalny_kolor);
        }
    }
};

// Klasa reprezentuj¹ca gracza
class Gracz {
public:
    sf::CircleShape ksztalt;   // jak gracz wygl¹da
    sf::Vector2i pozycja;      // gdzie jest
    sf::Vector2i kierunek;     // dok¹d idzie
    int punkty;                // ile punktów zdoby³
    std::string nazwa_profilu; // nazwa gracza

    // Konstruktor - ustawia pocz¹tkowe wartoœci gracza
    Gracz(const std::string& nazwa) : pozycja(1, 1), kierunek(0, 0), punkty(0), nazwa_profilu(nazwa) {
        ksztalt = sf::CircleShape(ROZMIAR_KAFELKA / 2 - 2);
        ksztalt.setFillColor(sf::Color::Yellow);
    }

    // Funkcja porusza graczem w aktualnym kierunku
    void ruch() {
        sf::Vector2i nowa_poz = pozycja + kierunek;
        // Sprawdzam czy mo¿na siê ruszyæ (nie ma œciany)
        if (mapa[nowa_poz.y][nowa_poz.x] != 1) {
            pozycja = nowa_poz;
        }
    }

    // Funkcja sprawdza czy gracz zebra³ punkt
    void aktualizuj_punkty(Punkt& punkt) {
        if (punkt.aktywny && punkt.pozycja == pozycja) {
            punkt.aktywny = false;  // punkt zebrany
            punkty += 10;           // dodajê 10 punktów
        }
    }

    // Funkcja zapisuje wynik do pliku
    void zapisz_wynik() {
        std::ofstream plik(nazwa_profilu + ".txt", std::ios::app);
        if (plik.is_open()) {
            plik << "Wynik: " << punkty << std::endl;
            plik.close();
        }
    }
};

// G³ówna funkcja programu
int main() {
    // Generator liczb losowych
    srand(static_cast<unsigned>(time(nullptr)));

    // Pobieranie nazwy gracza
    std::string nazwa_gracza;
    std::cout << "Podaj swoje imie: ";
    std::getline(std::cin, nazwa_gracza);

    // Tworzenie gracza
    Gracz gracz(nazwa_gracza);

    // Tworzenie okna gry
    sf::RenderWindow okno(sf::VideoMode(SZEROKOSC_OKNA, WYSOKOSC_OKNA), "Pac-Man");
    okno.setFramerateLimit(60);  // limit 60 klatek na sekundê

    // Zmienne do kontroli czasu i trybu gry
    sf::Clock zegar_gry;
    float czas_ostatniego_gonienia = 0.0f;
    bool tryb_gonienia = false;
    float czas_start_gonienia = 0.0f;
    bool pauza = false;

    // Tworzenie kszta³tu œciany
    sf::RectangleShape sciana(sf::Vector2f(ROZMIAR_KAFELKA, ROZMIAR_KAFELKA));
    sciana.setFillColor(sf::Color::Black);           // Czarne wype³nienie
    sciana.setOutlineColor(sf::Color::Blue);         // Niebieski obwód
    sciana.setOutlineThickness(2.0f);                // Gruboœæ obwodu - 2 piksele

    // Tworzenie punktów do zebrania
    std::vector<Punkt> punkty;
    int liczba_punktow = 0;
    // Przeszukujê mapê i dodajê punkty tam gdzie s¹ zera
    for (int y = 0; y < WYSOKOSC_MAPY; ++y) {
        for (int x = 0; x < SZEROKOSC_MAPY; ++x) {
            if (mapa[y][x] == 0) {
                punkty.push_back(Punkt(sf::Vector2i(x, y)));
                liczba_punktow++;
            }
        }
    }

    // Zmienna do kontroli prêdkoœci gry
    int licznik_klatek = 0;
    int opoznienie_ruchu = 15;  // co tyle klatek nastêpuje ruch

    // Tworzenie duchów
    std::vector<Duch> duchy = {
        Duch(sf::Color::Magenta, {10, 7}, {0, -1}),   // Ró¿owy duch
        Duch(sf::Color::Cyan, {10, 8}, {0, 1}),       // Niebieski duch
        Duch(sf::Color::Green, {1, 13}, {1, 0}),      // Zielony duch
        Duch(sf::Color(255, 165, 0), {19, 1}, {-1, 0})// Pomarañczowy duch
    };

    // Wczytywanie czcionki
    sf::Font czcionka;
    if (!czcionka.loadFromFile("arial.ttf")) {
        std::cout << "Blad" << std::endl;
        return -1;
    }

    // Tekst wyœwietlaj¹cy punkty
    sf::Text tekst_punktow;
    tekst_punktow.setFont(czcionka);
    tekst_punktow.setCharacterSize(30);
    tekst_punktow.setFillColor(sf::Color::White);
    tekst_punktow.setPosition(50, 0);

    // Tekst wyœwietlaj¹cy tryb gry
    sf::Text tekst_trybu;
    tekst_trybu.setFont(czcionka);
    tekst_trybu.setCharacterSize(30);
    tekst_trybu.setFillColor(sf::Color::White);
    tekst_trybu.setPosition(SZEROKOSC_OKNA - 250, 10);

    // Tekst wyœwietlany podczas pauzy
    sf::Text tekst_pauzy;
    tekst_pauzy.setFont(czcionka);
    tekst_pauzy.setCharacterSize(32);
    tekst_pauzy.setFillColor(sf::Color::Yellow);
    tekst_pauzy.setString("PAUZA\nR - Wznow\nQ - Wyjdz");
    tekst_pauzy.setPosition(SZEROKOSC_OKNA / 2 - 100, WYSOKOSC_OKNA / 2 - 50);

    // G³ówna pêtla gry
    while (okno.isOpen()) {
        // Obs³uga zdarzeñ (klawisze, zamkniêcie okna)
        sf::Event event;
        while (okno.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                okno.close();

            if (event.type == sf::Event::KeyPressed) {
                // Obs³uga pauzy
                if (event.key.code == sf::Keyboard::Escape) {
                    pauza = !pauza;
                }
                if (pauza) {
                    if (event.key.code == sf::Keyboard::R) {
                        pauza = false;
                    }
                    if (event.key.code == sf::Keyboard::Q) {
                        gracz.zapisz_wynik();
                        okno.close();
                    }
                }
                else {
                    // Obs³uga ruchu gracza
                    if (event.key.code == sf::Keyboard::W) gracz.kierunek = { 0, -1 };
                    if (event.key.code == sf::Keyboard::S) gracz.kierunek = { 0, 1 };
                    if (event.key.code == sf::Keyboard::A) gracz.kierunek = { -1, 0 };
                    if (event.key.code == sf::Keyboard::D) gracz.kierunek = { 1, 0 };
                }
            }
        }

        // Jeœli pauza, wyœwietl tylko tekst pauzy
        if (pauza) {
            okno.clear();
            okno.draw(tekst_pauzy);
            okno.display();
            continue;
        }

        // Sprawdzanie czasu do zmiany trybu gry
        float aktualny_czas = zegar_gry.getElapsedTime().asSeconds();

        // Zmiana trybu gry
        if (!tryb_gonienia && aktualny_czas - czas_ostatniego_gonienia >= PRZERWA_GONIENIA) {
            tryb_gonienia = true;
            czas_start_gonienia = aktualny_czas;
            czas_ostatniego_gonienia = aktualny_czas;
        }
        else if (tryb_gonienia && aktualny_czas - czas_start_gonienia >= CZAS_GONIENIA) {
            tryb_gonienia = false;
        }

        // Aktualizacja stanu gry co kilka klatek
        if (++licznik_klatek >= opoznienie_ruchu) {
            licznik_klatek = 0;

            // Ruch gracza
            gracz.ruch();

            // Sprawdzanie zebranych punktów
            for (auto& punkt : punkty) {
                gracz.aktualizuj_punkty(punkt);
            }

            // Sprawdzanie czy wszystkie punkty zebrane
            bool wszystkie_zebrane = true;
            for (const auto& punkt : punkty) {
                if (punkt.aktywny) {
                    wszystkie_zebrane = false;
                    break;
                }
            }
            if (wszystkie_zebrane) {
                gracz.zapisz_wynik();
                okno.close();
                std::cout << "Wygrana! Zdobyte punkty: " << gracz.punkty << std::endl;
            }

            // Aktualizacja duchów
            for (auto& duch : duchy) {
                duch.zmien_kierunek(gracz.pozycja, tryb_gonienia);
                sf::Vector2i nowa_poz = duch.pozycja + duch.kierunek;
                if (mapa[nowa_poz.y][nowa_poz.x] != 1) {
                    duch.pozycja = nowa_poz;
                }
                // Sprawdzanie kolizji z graczem
                if (duch.pozycja == gracz.pozycja) {
                    gracz.zapisz_wynik();
                    okno.close();
                    std::cout << "Koniec gry! Zdobyte punkty: " << gracz.punkty << std::endl;
                }
            }
        }

        // Rysowanie
        okno.clear();

        // Rysowanie œcian
        for (int y = 0; y < WYSOKOSC_MAPY; ++y) {
            for (int x = 0; x < SZEROKOSC_MAPY; ++x) {
                if (mapa[y][x] == 1) {
                    sciana.setPosition(x * ROZMIAR_KAFELKA, y * ROZMIAR_KAFELKA);
                    okno.draw(sciana);
                }
            }
        }

        // Rysowanie punktów
        for (const auto& punkt : punkty) {
            if (punkt.aktywny) {
                okno.draw(punkt.ksztalt);
            }
        }

        // Rysowanie gracza
        gracz.ksztalt.setPosition(
            gracz.pozycja.x * ROZMIAR_KAFELKA + 2,
            gracz.pozycja.y * ROZMIAR_KAFELKA + 2
        );
        okno.draw(gracz.ksztalt);

        // Rysowanie duchów
        for (auto& duch : duchy) {
            duch.ksztalt.setPosition(
                duch.pozycja.x * ROZMIAR_KAFELKA + 2,
                duch.pozycja.y * ROZMIAR_KAFELKA + 2
            );
            okno.draw(duch.ksztalt);
        }

        // Aktualizacja i rysowanie tekstów
        tekst_punktow.setString("Punkty: " + std::to_string(gracz.punkty));
        okno.draw(tekst_punktow);

        tekst_trybu.setString(tryb_gonienia ? "GONIENIE!" : "Normalny Tryb");
        tekst_trybu.setFillColor(tryb_gonienia ? sf::Color::Red : sf::Color::White);
        okno.draw(tekst_trybu);

        // Wyœwietlenie wszystkiego na ekranie
        okno.display();
    }
    return 0;
}