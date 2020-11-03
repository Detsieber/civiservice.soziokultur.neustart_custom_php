<?php
/**
 * Created by Andreas Howiller
 * Last update: -
 * Date: 13.10.2020
 * Time: 20:50
 * 
 */

/**
 * First, the available tokens are defined:
 * @param $token
 * @return array
 */
function nkprogramm_civitoken_declare($token){
  return array(
    $token. '.foerderbetraginworten' => 'Bewilligter Förderbetrag (in Worten)',
  );
}


/* Diese Funktion ist bereits in neustart.inc enthalten.
function inWorten...
*/


function _nkprogramm_get_case_id($value) {
  // Hack to get case id from the url
  if (!empty($_GET['caseid'])) {
    \Civi::$statics['nkprogramm']['case_id'] = $_GET['caseid'];
  }
  // Extra hack to get it from the entry url after a form is posted
  if (empty(\Civi::$statics['nkprogramm']['case_id']) && !empty($_POST['entryURL'])) {
    $matches = array();
    preg_match('#caseid=(\d+)#', $_POST['entryURL'], $matches);
    \Civi::$statics['nkprogramm']['case_id'] = CRM_Utils_Array::value(1, $matches);
  }
  if (empty(\Civi::$statics['nkprogramm']['case_id'])) {
   if (isset($value['case_id'])) {
      \Civi::$statics['nkprogramm']['case_id'] = $value['case_id'];
    }
    elseif (isset($value['case.id'])) {
      \Civi::$statics['nkprogramm']['case_id'] = $value['case.id'];
    }
  }
  return isset(\Civi::$statics['nkprogramm']['case_id']) ? \Civi::$statics['nkprogramm']['case_id'] : NULL;
}

/**
 * Now, the tokens are filled with their values:
 * @param $cid
 * @param $value
 */
function nkprogramm_civitoken_get($cid, &$value){

  $caseId = _nkprogramm_get_case_id($value);
  
/**
 * Ermitteln, des Förderbetrags in Worten:
 */

    $result = civicrm_api3('Case', 'get', [
      'sequential' => 1,
      'return' => ["custom_748"],
      'id' => $caseId,
    ]);
    $foerderbetrag = $result['values'][0]['custom_748'];
    $foerderbetraginworten = inWorten($foerderbetrag, 'EUR');

  /*  Fill in values to tokens
  */
  $value['nkprogramm.foerderbetraginworten'] = $foerderbetraginworten;

}
