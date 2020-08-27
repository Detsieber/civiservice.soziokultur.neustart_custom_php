<?php
/**
 * Created by Detlev Sieber
 * Last update: Andreas Howiller
 * Date: 12.08.2020
 * Time: 14:00
 * 
 */

/**
 * First, the available tokens are defined:
 * @param $token
 * @return array
 */
function neustart_civitoken_declare($token){
  return array(
    $token. '.textvmb' => 'Textbaustein für vorz. Maßnahmenbeginn',
    $token. '.texteigenanteil' => 'Textbaustein für Eigenanteil',
    $token. '.sonstigeauflagen' => 'Textbaustein für sonstige Auflagen',
    $token. '.foerderbetraginworten' => 'Bewilligter Förderbetrag (in Worten)',
    $token. '.gliederunganlageoptional' => 'Textbaustein Inhalt für optionale Anlagen',
    $token. '.absatzanlageoptional' => 'Textbaustein Absatz für optionale Anlagen',
    $token. '.kultureinrichtung' => 'Textbaustein Antrag für Kultureinrichtung',
    $token. '.bruttoodernetto' => 'Brutto oder Netto (ja nach Steuerangabe)',
  );
}


function inWorten($num) {
    $num = str_replace(array(',', ' '), '' , trim($num));
    if(!$num) {
      return false;
    }
    $num = (int) $num;
    $words = array();
    $list1 = array('', 'ein', 'zwei', 'drei', 'vier', 'fünf', 'sechs', 'sieben', 'acht', 'neun', 'zehn', 'elf',
      'zwölf', 'dreizehn', 'vierzehn', 'fünfzehn', 'sechzehn', 'siebzehn', 'achtzehn', 'neunzehn'
    );
    $list2 = array('', 'zehn', 'zwanzig', 'dreißig', 'vierzig', 'fünfzig', 'sechzig', 'siebzig', 'achtzig', 'neunzig', 'hundert');
    $list3 = array('', 'tausend', 'millionen', 'milliarden'
    );
    $num_length = strlen($num);
    $levels = (int) (($num_length + 2) / 3);
    $max_length = $levels * 3;
    $num = substr('00' . $num, -$max_length);
    $num_levels = str_split($num, 3);
    for ($i = 0; $i < count($num_levels); $i++) {
      $levels--;
      $hundreds = (int) ($num_levels[$i] / 100);
      $hundreds = ($hundreds ? ' ' . $list1[$hundreds] . ' hundert' . ' ' : '');
      $tens = (int) ($num_levels[$i] % 100);
      $singles = '';
      if ( $tens < 20 ) {
          $tens = ($tens ? ' ' . $list1[$tens] . ' ' : '' );
      } else {
          $tens = (int)($tens / 10);
          $tens = ' ' . $list2[$tens] . ' ';
          $singles = (int) ($num_levels[$i] % 10);
          $singles = ' ' . $list1[$singles] . ' und ';
      }
      $words[] = $hundreds .  $singles .$tens . ( ( $levels && ( int ) ( $num_levels[$i] ) ) ? ' ' . $list3[$levels] . ' ' : '' );
    } //end for loop
/*    $commas = count($words);
    if ($commas > 1) {
      $commas = $commas - 1;
    } */
    return implode(' ', $words);
  }


function _neustart_get_case_id() {
  // Hack to get case id from the url
  if (!empty($_GET['caseid'])) {
    \Civi::$statics['neustart']['case_id'] = $_GET['caseid'];
  }
  // Extra hack to get it from the entry url after a form is posted
  if (empty(\Civi::$statics['neustart']['case_id']) && !empty($_POST['entryURL'])) {
    $matches = array();
    preg_match('#caseid=(\d+)#', $_POST['entryURL'], $matches);
    \Civi::$statics['neustart']['case_id'] = CRM_Utils_Array::value(1, $matches);
  }
  return isset(\Civi::$statics['neustart']['case_id']) ? \Civi::$statics['neustart']['case_id'] : NULL;
}

/**
 * Now, the tokens are filled with their values:
 * @param $cid
 * @param $value
 */
function neustart_civitoken_get($cid, &$value){

$caseId = _neustart_get_case_id();

$textvmb = $caseId;
$texteigenanteil = $cid;

/**
 * Ermitteln, ob "Datum vorzeitiger Maßnahmenbeginn - Bewilligung" gefüllt ist:
 * Wenn ja, wird ein Textbaustein erzeugt.
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_393"],
      'id' => $caseId,
    ]);
    $textvmb = "";
    if ($result['values'][0]['custom_393'])
       $textvmb = '</td></tr><tr><td>&nbsp;</td><td>&nbsp;</td></tr><tr><td><strong>(2)</strong></td><td>Die
Ausnahme vom grunds&auml;tzlichen Verbot des Projektbeginns vor der
Bewilligung wurde Ihnen bereits seitens des Bundesverbandes mit Wirkung
zum '.
                  date_format(date_create($result['values'][0]['custom_393']), 'd.m.yy').
                   ' zugelassen.';
                   
/**
 * Ermitteln, ob "mehrere Kultureinrichtungen zulässig":
 * Wenn ja, wird ein anderer Textbaustein eingefügt.
 */

$result = civicrm_api3('Case', 'get', [
  'sequential' => 1,
  'return' => ["custom_430"],
  'id' => $caseId,
]);
$gliederunganlageoptional = "";
$absatzanlageoptional = ' sowie G - Fördergrundsätze der BKM für "NEUSTART. Sofortprogramm für Corona-bedingte Investitionen in Kultureinrichtungen"';
if ($result['values'][0]['custom_430'] == 1) {
  $gliederunganlageoptional = '</li><li>Anlage H - Grundzüge der Vergabe (Stand Juni 2018)</li>';
  $absatzanlageoptional = ', G - Fördergrundsätze der BKM für "NEUSTART. Sofortprogramm für Corona-bedingte Investitionen in Kultureinrichtungen" sowie H - Grundzüge der Vergabe (Stand Juni 2018)';

}

/**
 * Ermitteln, ob "Antragsteller und Kultureinrichtung stimmen überein":
 * Wenn nein, wird ein zusätzlicher Textbaustein eingefügt.
 */

$result = civicrm_api3('Case', 'get', [
  'sequential' => 1,
  'return' => ["custom_460"],
  'id' => $caseId,
]);
$kultureinrichtung = "";
if ($result['values'][0]['custom_460'] == 2) {
  $result = civicrm_api3('Case', 'get', [
    'sequential' => 1,
    'return' => ["custom_300"],
    'id' => $caseId,
  ]);
  $kultureinrichtung = '</td></tr><tr><td>&nbsp;</td><td style="border-bottom-style: solid; border-bottom-width:thin">für die Kultureinrichtung ' . $result['values'][0]['custom_300'];

}

/**
 * Ermitteln, ob ausnahmsweise der Eigenanteil von 10% überhauptnicht oder anteilig nicht erbracht werden muss:
 * Für alle drei Möglichkeiten wird jeweils ein anderer Textbaustein eingefügt.
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_425", "custom_330", "custom_333", "custom_334"],
      'id' => $caseId,
    ]);
    $gesamtausgaben = $result['values'][0]['custom_330'] + 0;
    $eigenmittel = $result['values'][0]['custom_333'] + $result['values'][0]['custom_334'];
    $texteigenanteil = "Die zuwendungsfähigen Gesamtausgaben des geförderten Projektes betragen insgesamt " .  $gesamtausgaben . " Euro. Davon finanziert der Letztempfänger insgesamt " .  $eigenmittel . " Euro durch Eigen- und/oder Drittmittel.";
    if ($result['values'][0]['custom_425'] == 1) {
       $texteigenanteil = "Die zuwendungsfähigen Gesamtausgaben des geförderten Projektes betragen insgesamt " . $gesamtausgaben . " Euro. Davon finanziert der Letztempfänger insgesamt " .  $eigenmittel . " Euro durch Eigen- und/oder Drittmittel. Ein begründeter Ausnahmefall für die Unterschreitung der Mindestbeteiligung liegt vor.";
    } else if ($result['values'][0]['custom_425'] == 4)
       $texteigenanteil = "Die zuwendungsfähigen Gesamtausgaben des geförderten Projektes betragen insgesamt " . $gesamtausgaben . " Euro. Der Letztempfänger bringt keine Eigen- und/oder Drittmittel zur Finanzierung ein. Ein begründeter Ausnahmefall für diese Ausnahme liegt vor.";

/**
 * Ermitteln, ob die Bewilligung Auflagen enthält.
 * Wenn ja, wird ein Textbaustein erzeugt.
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_439", "custom_440"],
      'id' => $caseId,
    ]);
    $sonstigeauflagen = "";
    if ($result['values'][0]['custom_439'] == 1)
       $sonstigeauflagen = '</td></tr><tr><td>&nbsp;</td><td>&nbsp;</td></tr><tr><td><strong>(12)</strong></td><td><strong>Sonstige
Vereinbarungen</strong><br />'.
                  $result['values'][0]['custom_440'];

/**
 * Ermitteln der Auswahl "Abzugsberechtigung Vorsteuer":
 * Wenn nein, Ausgabe "Brutto" wenn ja Ausgabe "Netto".
 */

$result = civicrm_api3('Case', 'get', [
  'sequential' => 1,
  'return' => ["custom_269"],
  'id' => $caseId,
]);
$bruttoodernetto = "Brutto/Netto";
if ($result['values'][0]['custom_269'] == 1) {
  $bruttoodernetto = "Netto";
} else if ($result['values'][0]['custom_269'] == 2) $bruttoodernetto = "Brutto";

/**
 * Ermitteln, des Förderbetrags in Worten:
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_338"],
      'id' => $caseId,
    ]);
    $foerderbetrag = $result['values'][0]['custom_338'];

// Test:
//$foerderbetrag = 1886799.08;

    $foerderbetragganzzahl = floor($foerderbetrag);
    $foerderbetraginworten = inWorten($foerderbetragganzzahl);

/*  Fill in values to tokens
 */
    $value['neustart.textvmb'] = $textvmb;
    $value['neustart.texteigenanteil'] = $texteigenanteil;
    $value['neustart.sonstigeauflagen'] = $sonstigeauflagen;
    $value['neustart.foerderbetraginworten'] = $foerderbetraginworten;
    $value['neustart.gliederunganlageoptional'] = $gliederunganlageoptional;
    $value['neustart.absatzanlageoptional'] = $absatzanlageoptional;
    $value['neustart.kultureinrichtung'] = $kultureinrichtung;
    $value['neustart.bruttoodernetto'] = $bruttoodernetto;
}
