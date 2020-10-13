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


/* Diese Funktion ist bereits in neustart.inc enthalten!
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
    } 
    return implode(' ', $words);
  }
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
    $foerderbetragganzzahl = floor($foerderbetrag);
    $foerderbetraginworten = inWorten($foerderbetragganzzahl);

  /*  Fill in values to tokens
  */
  $value['nkprogramm.foerderbetraginworten'] = $foerderbetraginworten;

}
