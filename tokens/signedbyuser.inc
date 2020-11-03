<?php
/**
 * Created by Detlev Sieber
 * Last update: Andreas Howiller
 * Last changes: added check for results of requests before filling variables to prevent dompdf from creating broken files
 * Date: 03.11.2020
 * Time: 15:08
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
    $token. '.email' => 'primäre E-Mail Unterzeichnende',
    $token. '.phone' => 'primäre Tel. Unterzeichnende',
    $token. '.signature_html' => 'E-Mail-Signatur (html) Unterzeichnende',
    $token. '.signature_text' => 'E-Mail-Signatur (Text) Unterzeichnende',
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


/*  Get name, email, phone and job_title of logged-in user via API
 */ 
    $result = civicrm_api3('Contact', 'get', array(
      'sequential' => 1,
      'return' => "display_name,job_title,email,phone",
      'id' => $currentuser,
    ));

    if(isset($result['values'][0]['display_name'])) {
      $display_name = $result['values'][0]['display_name'];
    } else $display_name = "";

    if(isset($result['values'][0]['job_title'])) {
      $job_title = $result['values'][0]['job_title'];
    } else $job_title = "";

    if(isset($result['values'][0]['primary_email'])) {
      $primary_email = $result['values'][0]['primary_email'];
    } else $primary_email = "";

    if(isset($result['values'][0]['primary_phone'])) {
      $primary_phone = $result['values'][0]['primary_phone'];
    } else $primary_phone = "";

		
/*  Get email signature (html) from primary email address of the logged-in contact
*/
    $result = civicrm_api3('Email', 'get', [
      'sequential' => 1,
      'return' => ["signature_html", "signature_text", "is_primary"],
      'contact_id.id' => $currentuser,
      'is_primary' => 1,
    ]);

    if(isset($result['values'][0]['signature_html'])) {
      $signature_html = $result['values'][0]['signature_html'];
    } else $signature_html = "";
    
    if(isset($result['values'][0]['signature_text'])) {
      $signature_text = $result['values'][0]['signature_text'];
    } else $signature_text = "";

/*  Fill in values to tokens
 */
    $value['signedbyuser.name'] = $display_name;
    $value['signedbyuser.email'] = $primary_email;
    $value['signedbyuser.phone'] = $primary_phone;
    $value['signedbyuser.function'] = $job_title;
    $value['signedbyuser.signature_html'] = $signature_html;
    $value['signedbyuser.signature_text'] = $signature_text;
}
