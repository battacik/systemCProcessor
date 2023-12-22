#include <systemc.h>

SC_MODULE(busint) {
    //Sygna�y wej�ciowe
    sc_in<sc_int<32>> ADR; //Adres WEJ�CIOWE
    sc_in<sc_int<16>> DO; //Dane WEJ�CIOWE (IN)

    //Sygana�y steruj�ce operacjami na magistrali
    sc_in<bool> Smar; // 0 - pami�tanie adresu, 1 - zapis
    sc_in<bool> Smbr; // 0 - pami�tanie danych, 1 - zapis
    sc_in<bool> WRin; // uruchom/zatrzymaj operacj� zapisu
    sc_in<bool> RDin; // uruchom/zatrzymaj operacj� odczytu

    //Sygna�y wyj�ciowe
    sc_out<sc_int<32>> AD; //Adres WYJ�CIOWE
    sc_inout<sc_int<16>> D; //Dane IN-OUT
    sc_out<sc_int<16>> DI; //Dane OUT
    sc_out<bool> WR, RD; //Informuj� czy aktualnie operacja zapisu (WR) czy odczytu (RD)

    //Proces g��wny
    void process() {

        // Zmienne wewn�trzne
        sc_int<16> MBRin, MBRout; //Dane odczytane i zapisane do magistrali
        sc_int<32> MAR; //Adres odczytany z magistrali

        //Logika poszczeg�lnych sygna��w steruj�cych magistrali
        if (Smar.read() == 1) { 
            MAR = ADR.read();
        }
        if (Smbr.read() == 1) {
            MBRout = DO.read();
        }
        if (RDin.read() == 1) {
            MBRin = D.read();
        }
        if (WRin.read() == 1) { //czy aktywowany sygna� zapisu
            D.write(MBRout);
        }
        else {
            D.write("ZZZZZZZZZZZZZZZZ"); //brak danych
        }

        DI.write(MBRin);
        AD.write(MAR);
        WR.write(WRin);
        RD.write(RDin);
    }

    //Konstruktor
    SC_CTOR(busint) {
        SC_METHOD(process);
        sensitive << Smar << ADR << Smbr << DO << D << WRin << RDin;
    }
};

int sc_main(int argc, char* argv[]) {
    // Deklaracje sygna��w
    sc_signal<sc_int<32>> ADR;
    sc_signal<sc_int<16>> DO, D, DI;
    sc_signal<bool> Smar, Smbr, WRin, RDin, WR, RD;

    // Utworzenie instancji modu�u
    busint busint("MyBusint");

    // Po��czenie sygna��w
    busint.ADR(ADR);
    busint.DO(DO);
    busint.Smar(Smar);
    busint.Smbr(Smbr);
    busint.WRin(WRin);
    busint.RDin(RDin);
    busint.AD(ADR);
    busint.D(D);
    busint.DI(DI);
    busint.WR(WR);
    busint.RD(RD);

    // Przyk�adowe warto�ci sygna��w
    ADR.write(0b00001); //przyk�adowo adres 3
    DO.write(0b110001); //przyk�adowo dane 49

    Smar.write(true); //zapis adresu
    Smbr.write(true); //zapis danych
    WRin.write(true); //uruchomienie operacji zapisu
    RDin.write(true); //uruchomienie operacji odczytu

    // Uruchomienie symulacji
    sc_start(100, SC_NS);

    std::cout << "\nOdczytane dane: " << busint.DI.read() << " z adresu: " << busint.AD.read();

    return 0;
}
