# Bezdrátová požární časomíra
Bezdrátová časomíra pro požární sport řízená pomocí platformy Arduino. Obsahuje menu s výběrem možností startu požárního útoku, nastavitelným odpočtem přípravného času a historii naměřených časů uložených na SD kartě.

Seznam modulů řídící jednotky:
- Arduino Mega Pro Mini
- HC-12 RF module
- MicroSD Card module
- DS1307 RTC modul
- Nabíjecí modul pro Li-Ion články TP4056
- Step-Up modul MT3608

Seznam modulů jednotky u terčů:
- Arduino Nano
- HC-12 RF module
- Nabíjecí modul pro Li-Ion články TP4056
- Step-Up modul MT3608
 
      
Po sepnutí hlavního spínače se zkontroluje správná komunikace se všemi moduly. V případě chybné komunikace se vypíše oznámení na displej. Požární útok nebude možný provést bez úspěšné komunikace s modulem u terčů. V případě, že chybí pouze SD karta nebo RTC modul, útok bude možné provést, ale nebude zaznamenávan. 

Základní menu:

- Na displeji se zobrazuje šipka, která značí výběr možnosti v momentálním menu. Šipka se může posouvat mezi možnostmi pomocí dvou tlačítek "Nahoru" a "Dolů", výběr se potvrdí stisknutím tlačítka "OK".
- Na každém menu se nacházi možnost "Zpět" pro návrat na předchozí menu.

=> Požární útok
- Zde je na výběr způsob startu požárního útoku. Na výber jsou dvě možnosti, automatický způsob startu a manuální. Při potvrzení jedné nebo druhé možnosti se ještě přejde do menu pro potvrzení příslušného způsobu. Potvrzením možnosti "Spustit" se spustí časomíra. V automatickém způsobu startu se ještě před spuštěním časomíry objeví upozornění pro závodníky k připravení na start. Následuje náhodný odpočet a po signalizaci bzučákem je útok odstartován. V manuálním režimu se spustí časomíra hned po potvrzení možnosti "Spustit". Na displeji se zobrazuje momentální čas od spuštění časomíry. Při obdržení zprávy z jednotky u terčů se zapíše na displej příslušný teřč a čas jeho shození. Požární útok se dokončí po shození obou terčů. Výsledný čas je ten pomalejší z nich. Na SD kartu se zapíše výsledný čas, časy obou terčů a také datum a čas provedení útoku. Každému útoku je přiřazeno patřičné ID. Hned po provedení útoku ho bude možné v tomto menu smazat.

=> Odpočet přípravy
- Nastavitelný čas odpočtu přípravy základny. Maximálně lze nastavit 6 minut. Odpočet lze v případě potřeby přerušit stisknutím tlačítka "OK". Po skončení odpočtu následuje zvukový signál bzučáku a přejde se zpět do základního menu. 

=> Historie naměřených časů
V tomto menu se vypisujou informace uložené na SD kartě. Každý útok má svoje ID a pod ním je zaznamenávan datum a čas útoků, oba časy shozených terčů a výsledný (pomalejší) z nich. Záznamy jsou v tomto menu seřazené podle data provedení útoku. Přepínat na předchozí nebo následující záznam lze pomocí výberu možnosti u specifických šipek. Záznamy je možné vymazat výběrem možnosti "Smazat" a následným potvrzením, že opravdu chceme smazat tento záznam. 

=> Nejrychlejší útoky
- Záznamy jsou zde seřazeny vzestupně od nejrychlejšího výsledného času. Záznamy zde lze také smazat.


Bezdrátové moduly:

- Přenos na dálku je zaručen moduly HC-12, které pracují na frekvenci 433MHz. Moduly jsou nastaveny na nejvyšší přenosovou rychlost s dostatečným dosahem.

Možnost nabíjení:

- Článek Li-Ion 18650 se jmenovitým napětím 3,7V se může nabíjet pomocí modulu TP4056 s ochranou. Napětí na 5V zaručí step-up modul MT3608. Nabíjet lze micro-USB konektorem, nebo jiným 5V zdrojem na příslušné kontakty.
- Nabíjet lze se zapnutým i vypnutým hlavním spínačem. Při nabíjení a zároveň používání časomíry se jednotka napájí z externího zdroje a tedy z Li-Ion článku se neodebírá žádný proud.

Indikace stavu baterie:

- Snímaní hodnoty napětí baterie z výstupu nabíjecího modulu na analogový vstup arduina.
- Přes rezistor s velkým odporem pro malý ztrátový výkon
- Indikace baterie řídící jednotky i jednotky u terčů na displeji řídící jednotky.
- Několik stavů pro zobrazení momentální hodnoty, indikace jestli se baterie nabíjí, nebo nenabíjí.
