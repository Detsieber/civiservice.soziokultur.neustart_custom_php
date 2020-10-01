<?php
/**
 * Created by Andreas Howiller
 * Last update: Detlev Sieber
 * Date: 21.09.2020
 * Time: 14:08
 * Wird ersetzt durch nkzentren.inc
 * 
 */

/**
 * First, the available tokens are defined:
 * @param $token
 * @return array
 */
function neustartkultur_civitoken_declare($token){
  return array(
    $token. '.bruttoodernetto' => 'brutto oder netto (ja nach Veranlagung)',
    $token. '.umsatzsteuerabgezogen' => 'Baustein Ust abgezogen (je nach Veranlagung)',
  );
}

function _neustartkultur_get_case_id($value) {
  // Hack to get case id from the url
  if (!empty($_GET['caseid'])) {
    \Civi::$statics['neustartkultur']['case_id'] = $_GET['caseid'];
  }
  // Extra hack to get it from the entry url after a form is posted
  if (empty(\Civi::$statics['neustartkultur']['case_id']) && !empty($_POST['entryURL'])) {
    $matches = array();
    preg_match('#caseid=(\d+)#', $_POST['entryURL'], $matches);
    \Civi::$statics['neustartkultur']['case_id'] = CRM_Utils_Array::value(1, $matches);
  }
  if (empty(\Civi::$statics['neustartkultur']['case_id'])) {
   if (isset($value['case_id'])) {
      \Civi::$statics['neustartkultur']['case_id'] = $value['case_id'];
    }
    elseif (isset($value['case.id'])) {
      \Civi::$statics['neustartkultur']['case_id'] = $value['case.id'];
    }
  }
  return isset(\Civi::$statics['neustartkultur']['case_id']) ? \Civi::$statics['neustartkultur']['case_id'] : NULL;
}

/**
 * Now, the tokens are filled with their values:
 * @param $cid
 * @param $value
 */
function neustartkultur_civitoken_get($cid, &$value){

  $caseId = _neustartkultur_get_case_id($value);

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


  /*  Fill in values to tokens
  */
  $value['neustartkultur.bruttoodernetto'] = $bruttoodernetto;
  $value['neustartkultur.umsatzsteuerabgezogen'] = $umsatzsteuerabgezogen;
}
