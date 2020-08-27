<?php
/**
 * Created by Andreas Howiller
 * Last update: Andreas Howiller
 * Date: 27.08.2020
 * Time: 11:08
 * 
 */

/**
 * First, the available tokens are defined:
 * @param $token
 * @return array
 */
function neustartkultur_civitoken_declare($token){
  return array(
    $token. '.bruttoodernetto' => 'Brutto oder Netto (ja nach Veranlagung)',
    $token. '.umsatzsteuerabgezogen' => 'Baustein Ust abgezogen (je nach Veranlagung)',
  );
}

function _neustartkultur_get_case_id() {
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
function neustartkultur_civitoken_get($cid, &$value){

$caseId = _neustartkultur_get_case_id();

/**
 * Ermitteln der Auswahl "Abzugsberechtigung Vorsteuer":
 * Wenn nein, Ausgabe "Brutto" wenn ja Ausgabe "Netto" sowie generierung Textbaustein zu Umsatzsteuerabzug
 */

$result = civicrm_api3('Case', 'get', [
  'sequential' => 1,
  'return' => ["custom_269"],
  'id' => $caseId,
]);
$bruttoodernetto = "Brutto/Netto";
$umsatzsteuerabgezogen = "";
if ($result['values'][0]['custom_269'] == 1) {
  $bruttoodernetto = "Netto";
  $umsatzsteuerabgezogen = "Nach § 15 des UStG als Vorsteuer abziehbare Umsatzsteuer wurde abgezogen.";
} else if ($result['values'][0]['custom_269'] == 2) $bruttoodernetto = "Brutto";

/*  Fill in values to tokens
 */
    $value['neustartkultur.bruttoodernetto'] = $bruttoodernetto;
    $value['neustartkultur.umsatzsteuerabgezogen'] = $umsatzsteuerabgezogen;
}
