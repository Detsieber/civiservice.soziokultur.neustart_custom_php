<?php
/**
 * Created by Detlev Sieber
 * Last update: Detlev
 * Date: 16.6.2017
 * Time: 15:24
 */

/**
 * First, the available tokens are defined:
 * @param $token
 * @return array
 */
function signedbyuser_civitoken_declare($token){
  return array(
    $token. '.name' => 'Name Unterzeichnende',
    $token. '.function' => 'Funktion Unterzeichnende',
    $token. '.signature_html' => 'E-Mail-Signatur (html) Unterzeichnende',


  );
}


/**
 * Now, the tokens are filled with their values:
 * @param $cid
 * @param $value
 */
function signedbyuser_civitoken_get($cid, &$value){

/* Get contact_id of logged-in user 
 */
    $currentuser = CRM_Core_Session::singleton()->getLoggedInContactID();


/*  Get name and job_title of logged-in user via API
 */ 
    $result = civicrm_api3('Contact', 'get', array(
      'sequential' => 1,
      'return' => "display_name,job_title",
      'id' => $currentuser,
    ));

    $display_name = $result['values'][0]['display_name'];
    $job_title = $result['values'][0]['job_title'];

	
/*  Get email signature (html) from primary email address of the logged-in contact
*/
    $result = civicrm_api3('Email', 'get', [
      'sequential' => 1,
      'return' => ["signature_html", "signature_text", "is_primary"],
      'contact_id.id' => $currentuser,
      'is_primary' => 1,
    ]);

    $signature_html = $result['values'][0]['signature_html'];



/*  Fill in values to tokens
 */
    $value['signedbyuser.name'] = $display_name;
    $value['signedbyuser.function'] = $job_title;
    $value['signedbyuser.signature_html'] = $signature_html;



}
