/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/*  (C)Copyright IBM Corp.  2007, 2011  */
/**
 * \file src/coll/bcast/mpido_ibcast.c
 * \brief ???
 */

/*#define TRACE_ON*/

#include <mpidimpl.h>

int MPIDO_Ibcast(void *buffer,
                 int count,
                 MPI_Datatype datatype,
                 int root,
                 MPID_Comm *comm_ptr,
                 MPID_Request **request)
{
   TRACE_ERR("in mpido_ibcast\n");

   const unsigned is_root_rank = (comm_ptr->rank == root);
   const unsigned user_selected_type =
     comm_ptr->mpid.user_selected_type[PAMI_XFER_BROADCAST];

   int data_size, data_contig;
   MPI_Aint data_true_lb = 0;
   MPID_Datatype *data_ptr;

   MPIDI_Datatype_get_info(count, datatype,
               data_contig, data_size, data_ptr, data_true_lb);

   /*
    * If the user has constructed some weird 0-length datatype but
    * count is not 0, or if the user forced a mpich bcast alogorith,
    * perform a mpich bcast.
    */
   /*if (unlikely((data_size == 0) || (user_selected_type == MPID_COLL_USE_MPICH)))*/
   {
      /*
       * If the mpich mpir non-blocking collectives are enabled, return without
       * first constructing the MPID_Request. This signals to MPIR_Ibcast_impl
       * to invoke the mpich nbc implementation of MPI_Ibcast().
       */
      if (MPIDI_Process.mpir_nbc != 0)
       return 0;

      /*
       * MPIR_* nbc implementation is not enabled. Fake a non-blocking
       * MPIR_Ibcast() with a blocking MPIR_Bcast().
       */
      if(unlikely(MPIDI_Process.verbose >= MPIDI_VERBOSE_DETAILS_ALL && comm_ptr->rank == 0))
         fprintf(stderr,"Using MPICH bcast algorithm\n");

      int mpierrno = 0;
      int rc = MPIR_Bcast_intra(buffer, count, datatype, root, comm_ptr, &mpierrno);

      /*
       * The blocking bcast has completed - create and complete a MPID_Request
       * object so the MPIR_Ibcast_impl() function does not perform the bcast.
       */
      MPID_Request * mpid_request = MPID_Request_create_inline();
      mpid_request->kind = MPID_COLL_REQUEST;
      *request = mpid_request;
      MPIDI_Request_complete_norelease_inline(mpid_request);

      return rc;
   }

   TRACE_ERR("leaving ibcast\n");
   return 0;
}
