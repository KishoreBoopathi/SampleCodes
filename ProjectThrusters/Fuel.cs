using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Fuel : MonoBehaviour
{    
    private void OnTriggerEnter(Collider other) 
    {
        Destroy(this.gameObject);
    }
}
