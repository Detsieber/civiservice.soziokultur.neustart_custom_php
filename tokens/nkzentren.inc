<?php
/**
 * Created by Andreas Howiller
 * Last update: Andreas Howiller
 * Date: 25.09.2020
 * Time: 10:30
 * 
 */

/**
 * First, the available tokens are defined:
 * @param $token
 * @return array
 */
function nkzentren_civitoken_declare($token){
  return array(
    $token. '.bruttoodernetto' => 'brutto oder netto (je nach Veranlagung)',
    $token. '.umsatzsteuerabgezogen' => 'Baustein Ust abgezogen (je nach Veranlagung)',
    $token. '.textvmb' => 'Textbaustein für vorz. Maßnahmenbeginn',
    $token. '.kultureinrichtung' => 'Name der Kultureinrichtung, falls abweichender Name',
    $token. '.texteigenanteil' => 'Textbaustein für Eigenanteil',
    $token. '.foerderbetraginworten' => 'Bewilligter Förderbetrag (in Worten)',
    $token. '.sonstigeauflagen' => 'Textbaustein für sonstige Auflagen',
    $token. '.enddatumplus2monate' => 'Geplantes Fertigstellungsdatum plus 2 Monate',
    $token. '.pruefvermerkergebnis' => 'Prüfvermerk: Textbaustein für bewilligt/abgelehnt',
  );
}


/* Diese Funktion ist bereits in neustart.inc enthalten!
function inWorten...
*/


function _nkzentren_get_case_id($value) {
  // Hack to get case id from the url
  if (!empty($_GET['caseid'])) {
    \Civi::$statics['nkzentren']['case_id'] = $_GET['caseid'];
  }
  // Extra hack to get it from the entry url after a form is posted
  if (empty(\Civi::$statics['nkzentren']['case_id']) && !empty($_POST['entryURL'])) {
    $matches = array();
    preg_match('#caseid=(\d+)#', $_POST['entryURL'], $matches);
    \Civi::$statics['nkzentren']['case_id'] = CRM_Utils_Array::value(1, $matches);
  }
  if (empty(\Civi::$statics['nkzentren']['case_id'])) {
   if (isset($value['case_id'])) {
      \Civi::$statics['nkzentren']['case_id'] = $value['case_id'];
    }
    elseif (isset($value['case.id'])) {
      \Civi::$statics['nkzentren']['case_id'] = $value['case.id'];
    }
  }
  return isset(\Civi::$statics['nkzentren']['case_id']) ? \Civi::$statics['nkzentren']['case_id'] : NULL;
}

/**
 * Now, the tokens are filled with their values:
 * @param $cid
 * @param $value
 */
function nkzentren_civitoken_get($cid, &$value){

  $caseId = _nkzentren_get_case_id($value);

  $textvmb = $caseId;
  $texteigenanteil = $cid;
  
/**
 * Ermitteln, ob "Datum vorzeitiger Maßnahmenbeginn - Bewilligung" gefüllt ist:
 * Wenn ja, wird ein Textbaustein erzeugt.
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_533"],
      'id' => $caseId,
    ]);
    $textvmb = "";
    if ($result['values'][0]['custom_533'])
       $textvmb = '</td></tr><tr><td>&nbsp;</td><td>&nbsp;</td></tr><tr><td><strong>(2)</strong></td><td>Die
Ausnahme vom grunds&auml;tzlichen Verbot des Projektbeginns vor der
Bewilligung wurde Ihnen bereits seitens des Bundesverbandes mit Wirkung
zum '.
                  date_format(date_create($result['values'][0]['custom_533']), 'd.m.yy').
                   ' zugelassen.';
                   



  /**
   * Ermitteln der Auswahl "Abzugsberechtigung Vorsteuer":
   * Wenn nein, Ausgabe "brutto" wenn ja Ausgabe "netto" sowie generierung Textbaustein zu Umsatzsteuerabzug
   */

  $result = civicrm_api3('Case', 'get', [
    'sequential' => 1,
    'return' => ["custom_269"],
    'id' => $caseId,
  ]);
  $bruttoodernetto = "brutto/netto";
  $umsatzsteuerabgezogen = "";
  if ($result['values'][0]['custom_269'] == 1) {
    $bruttoodernetto = "netto";
    $umsatzsteuerabgezogen = "Nach § 15 des UStG als Vorsteuer abziehbare Umsatzsteuer wurde abgezogen.";
  } else if ($result['values'][0]['custom_269'] == 2) {
    $bruttoodernetto = "brutto";
  }

/**
 * Ermitteln, ob "Antragsteller und Kultureinrichtung stimmen überein":
 * Wenn nein, wird ein zusätzlicher Textbaustein eingefügt.
 */

$result = civicrm_api3('Case', 'get', [
  'sequential' => 1,
  'return' => ["custom_690"],
  'id' => $caseId,
]);
$kultureinrichtung = "";
if ($result['values'][0]['custom_690'] == 2) {
  $result = civicrm_api3('Case', 'get', [
    'sequential' => 1,
    'return' => ["custom_537"],
    'id' => $caseId,
  ]);
  $kultureinrichtung = '</td></tr><tr><td>&nbsp;</td><td style="border-bottom-style: solid; border-bottom-width:thin">für die Kultureinrichtung ' . $result['values'][0]['custom_537'];
}

/**
 * Ermitteln, ob ausnahmsweise der Eigenanteil von 10% überhaupt nicht oder anteilig nicht erbracht werden muss:
 * Für alle drei Möglichkeiten wird jeweils ein anderer Textbaustein eingefügt.
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_664", "custom_569", "custom_570", "custom_571"],
      'id' => $caseId,
    ]);
    $gesamtausgaben = $result['values'][0]['custom_569'] + 0;
    $eigenmittel = $result['values'][0]['custom_570'] + $result['values'][0]['custom_571'];
    $texteigenanteil = "Die zuwendungsfähigen Gesamtausgaben des geförderten Projektes betragen insgesamt " .  $gesamtausgaben . " Euro. Davon finanziert der Letztempfänger insgesamt " .  $eigenmittel . " Euro durch Eigen- und/oder Drittmittel.";
    if ($result['values'][0]['custom_664'] == 1) {
       $texteigenanteil = "Die zuwendungsfähigen Gesamtausgaben des geförderten Projektes betragen insgesamt " . $gesamtausgaben . " Euro. Davon finanziert der Letztempfänger insgesamt " .  $eigenmittel . " Euro durch Eigen- und/oder Drittmittel. Ein begründeter Ausnahmefall für die Unterschreitung der Mindestbeteiligung liegt vor.";
    } else if ($result['values'][0]['custom_664'] == 4)
       $texteigenanteil = "Die zuwendungsfähigen Gesamtausgaben des geförderten Projektes betragen insgesamt " . $gesamtausgaben . " Euro. Der Letztempfänger bringt keine Eigen- und/oder Drittmittel zur Finanzierung ein. Ein begründeter Ausnahmefall für diese Ausnahme liegt vor.";

/**
 * Ermitteln, des Förderbetrags in Worten:
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_580"],
      'id' => $caseId,
    ]);
    $foerderbetrag = $result['values'][0]['custom_580'];

// Test:
//$foerderbetrag = 1886799.08;

    $foerderbetraginworten = inWorten($foerderbetrag, 'EUR');


/**
 * Ermitteln, ob die Bewilligung Auflagen enthält.
 * Wenn ja, wird ein Textbaustein erzeugt.
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_678", "custom_679"],
      'id' => $caseId,
    ]);
    $sonstigeauflagen = "";
    if ($result['values'][0]['custom_678'] == 1)
       $sonstigeauflagen = '</td></tr><tr><td>&nbsp;</td><td>&nbsp;</td></tr><tr><td><strong>(14)</strong></td><td><strong>Sonstige
Vereinbarungen</strong><br />'.
                  $result['values'][0]['custom_679'];


/**
 * Ermitteln, des geplanten Fertigstellungsdatums zuzüglich 2 Monate:
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_529"],
      'id' => $caseId,
    ]);
    $enddatum_time = strtotime($result['values'][0]['custom_529']);
    
// Der folgende Code berücksichtigt keine kürzeren Monate, also z.B.: 
// 30.12.2020 + 2 Monate = 2.3.2021
// Das könnte in verwaltungsjuristischen Zusammenhängen nicht beabsichtigt sein.
    $enddatumplus2monate = date('d.m.Y', strtotime("+2 Months", $enddatum_time));

/**
 * Textbaustein ausgeben für das Ergebnis des Prüfvermerks (bewilligt/abgelehnt)
 */
    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["status_id", "custom_580", "custom_680", "custom_681"],
      'id' => $caseId,
    ]);
    $pruefvermerkergebnis = "Fehler: Der Status des Falls muss entweder Abgelehnt oder Bewilligt sein um die gew&uuml;nschte Angabe ausgeben zu k&ouml;nnen.";
    if ($result['values'][0]['status_id'] == 6){
       $pruefvermerkergebnis = '<strong><em>Nach Aussch&ouml;pfung aller vertretbaren Pr&uuml;fungsm&ouml;glichkeiten werden die Kosten nicht als notwendig und angemessen anerkannt.</em></strong><strong><br /><br />Die Zuwendung kann nicht bewilligt werden, da</strong>';
       }
    elseif ($result['values'][0]['status_id'] == 5){
       $pruefvermerkergebnis = '<p><strong>Die Zuwendung kann als Festbetragsfinanzierung bewilligt werden gem&auml;&szlig;</strong><br /><br /> ['.$result['values'][0][custom_680].' '.$result['values'][0][custom_681].'] </p>
<p>in H&ouml;he von '.$result['values'][0][custom_580].'&nbsp;Euro.</p>
<p><strong><em>Nach Aussch&ouml;pfung aller vertretbaren Pr&uuml;fungsm&ouml;glichkeiten werden die Kosten als notwendig und angemessen anerkannt.</em></strong></p>';
       };



  /*  Fill in values to tokens
  */
  $value['nkzentren.textvmb'] = $textvmb;
  $value['nkzentren.bruttoodernetto'] = $bruttoodernetto;
  $value['nkzentren.umsatzsteuerabgezogen'] = $umsatzsteuerabgezogen;
  $value['nkzentren.kultureinrichtung'] = $kultureinrichtung;
  $value['nkzentren.texteigenanteil'] = $texteigenanteil;
  $value['nkzentren.foerderbetraginworten'] = $foerderbetraginworten;
  $value['nkzentren.sonstigeauflagen'] = $sonstigeauflagen;
  $value['nkzentren.enddatumplus2monate'] = $enddatumplus2monate;
  $value['nkzentren.pruefvermerkergebnis'] = $pruefvermerkergebnis;

}
